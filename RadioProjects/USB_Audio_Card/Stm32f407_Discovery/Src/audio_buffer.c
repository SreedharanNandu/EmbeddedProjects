/***********************************************************************

                  -- USB-DAC for STM32F4 Discovery --

------------------------------------------------------------------------

	Audio stream buffer

	   
	This file is licensed under the MIT License. See /LICENSE.TXT file.

***********************************************************************/

#include "audio_buffer.h"

#include <math.h>
#include <stdint.h>

/* =====================================================================
 *  EQ MODE SWITCH  (compile-time)
 *    EQ_MODE_OFF    : no EQ, original pass-through (bit-exact with the stock code)
 *    EQ_MODE_LEGACY : previous version - ONE low shelf, hard saturation, no high-pass
 *    EQ_MODE_DEEP   : new version - low shelf + high-pass (removes rumble) + soft limiter
 *
 *  Select by editing the line below, or define EQ_MODE in the project
 *  (CubeIDE: Properties > C/C++ Build > Settings > MCU GCC Compiler >
 *   Preprocessor > Defined symbols, e.g.  EQ_MODE=2 ).
 * ===================================================================== */
#define EQ_MODE_OFF      0
#define EQ_MODE_LEGACY   1
#define EQ_MODE_DEEP     2

#ifndef EQ_MODE
#define EQ_MODE          EQ_MODE_LEGACY//EQ_MODE_DEEP      /* <<<<<< CHANGE HERE */
#endif

/* ---- LEGACY mode settings (the previous setup) ---- */
#define EQ_LEG_HZ        22.0f     /* low-shelf corner Hz */
#define EQ_LEG_GAIN_DB   40.0f     /* low-shelf gain dB   */
#define EQ_LEG_PREAMP_DB (-24.0f)  /* preamp dB (headroom) */

/* ---- DEEP mode settings ---- */
#define EQ_DEEP_HZ        22.0f
#define EQ_DEEP_GAIN_DB   48.0f
#define EQ_DEEP_PREAMP_DB (-24.0f)
#define EQ_DEEP_HPF_HZ    20.0f    /* high-pass: keeps infrasonic content from railing the output */

#if (EQ_MODE != EQ_MODE_OFF)

#if (EQ_MODE == EQ_MODE_DEEP)
#define EQ_STAGES   2              /* 0: low shelf, 1: high-pass */
#else
#define EQ_STAGES   1              /* 0: low shelf only */
#endif

typedef struct { float b0,b1,b2,a1,a2, x1,x2,y1,y2; } biquad_t;
static biquad_t eq_l[EQ_STAGES], eq_r[EQ_STAGES];

static void eq_store(int st, float b0, float b1, float b2, float a0, float a1, float a2, float pre_db){
	float pre = powf(10.0f, pre_db / 20.0f);
	eq_l[st].b0 = eq_r[st].b0 = pre * b0 / a0;
	eq_l[st].b1 = eq_r[st].b1 = pre * b1 / a0;
	eq_l[st].b2 = eq_r[st].b2 = pre * b2 / a0;
	eq_l[st].a1 = eq_r[st].a1 = a1 / a0;
	eq_l[st].a2 = eq_r[st].a2 = a2 / a0;
	eq_l[st].x1 = eq_l[st].x2 = eq_l[st].y1 = eq_l[st].y2 = 0;
	eq_r[st].x1 = eq_r[st].x2 = eq_r[st].y1 = eq_r[st].y2 = 0;
}

/* RBJ low shelf. gain_db is reached far below f0 (half of it at f0). */
static void eq_lowshelf(int st, float fs, float f0, float gain_db, float pre_db){
	float A  = powf(10.0f, gain_db / 40.0f);
	float w0 = 2.0f * 3.14159265f * f0 / fs;
	float cs = cosf(w0), sn = sinf(w0);
	float t  = 2.0f * sqrtf(A) * (sn / 2.0f * 1.41421356f);   /* slope S = 1 */
	eq_store(st,      A*((A+1) - (A-1)*cs + t),
	           2.0f*A*((A-1) - (A+1)*cs),
	                  A*((A+1) - (A-1)*cs - t),
	                    (A+1) + (A-1)*cs + t,
	            -2.0f*((A-1) + (A+1)*cs),
	                    (A+1) + (A-1)*cs - t, pre_db);
}

#if (EQ_MODE == EQ_MODE_DEEP)
/* 2nd-order Butterworth high-pass */
static void eq_highpass(int st, float fs, float f0, float pre_db){
	float w0 = 2.0f * 3.14159265f * f0 / fs;
	float cs = cosf(w0), al = sinf(w0) / (2.0f * 0.70710678f);
	eq_store(st, (1+cs)/2, -(1+cs), (1+cs)/2, 1+al, -2*cs, 1-al, pre_db);
}
#endif

static inline int32_t eq_process(biquad_t *f, int32_t in){
	float x = (float)in;
	for (int s = 0; s < EQ_STAGES; s++){
		float y = f[s].b0*x + f[s].b1*f[s].x1 + f[s].b2*f[s].x2 - f[s].a1*f[s].y1 - f[s].a2*f[s].y2;
		f[s].x2 = f[s].x1; f[s].x1 = x;
		f[s].y2 = f[s].y1; f[s].y1 = y;
		x = y;
	}
#if (EQ_MODE == EQ_MODE_DEEP)
	{	/* soft limiter: linear below 70 % of full scale, smooth tanh knee above */
		const float FSC = 2147483648.0f, T = 0.7f * 2147483648.0f;
		float ax = fabsf(x);
		if (ax > T){
			float k = FSC - T;
			float lim = T + k * tanhf((ax - T) / k);
			x = (x < 0) ? -lim : lim;
		}
	}
#endif
	if (x >  2147483520.0f) return  2147483647;     /* hard saturation (LEGACY: this is the only protection) */
	if (x < -2147483520.0f) return -2147483647 - 1;
	return (int32_t)x;
}

static void eq_init(void){
#if (EQ_MODE == EQ_MODE_DEEP)
	eq_lowshelf(0, 44100.0f, EQ_DEEP_HZ, EQ_DEEP_GAIN_DB, EQ_DEEP_PREAMP_DB);
	eq_highpass(1, 44100.0f, EQ_DEEP_HPF_HZ, 0.0f);
#else
	eq_lowshelf(0, 44100.0f, EQ_LEG_HZ, EQ_LEG_GAIN_DB, EQ_LEG_PREAMP_DB);
#endif
}
#endif /* EQ_MODE != EQ_MODE_OFF */

/* ｵｰﾃﾞｨｵﾊﾞｯﾌｧ */
struct {
	unsigned char data[AUDIO_BUFFER_SIZE];
	unsigned int wp;
} audio_buffer __attribute__ ((aligned(128)));

/* @memo ﾊﾞｯﾌｧ上のﾌﾚｰﾑ長は8ﾊﾞｲﾄ…32bitｽﾃﾚｵ…固定 */

/* ﾊﾞｯﾌｧとは別に、ﾊﾞｯﾌｧへのﾃﾞｰﾀ流入出数を管理するｶｳﾝﾀ */
volatile signed int audio_buffer_count = 0;			/* in/out両方で更新する値 */
volatile signed int audio_buffer_count_fixed = 0;	/* out側だけで更新する値(out更新時に上記値をｺﾋﾟｰ)。ﾊﾞｯﾌｧ残数のﾌｨｰﾄﾞﾊﾞｯｸ通知算出用 */
volatile signed int audio_buffer_count_valid = 0;	/* audio_buffer_count同様だが、audio_buffer_fill_next_ip()での投入分が反映されない。その他の方法を用いて投入すると、あちら側と同値に修正される */


/*@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@*/
/*
	初期化
*/
void audio_buffer_init(){
	
	int size = sizeof(audio_buffer.data);
	int *p = (int *)audio_buffer.data;
	
	audio_buffer.wp = 0;
	audio_buffer_count = 0;
	audio_buffer_count_valid = 0;
	
	/* ｲﾝｼﾞｹｰﾀ更新 */
	(void)audio_buffer_read(0, 0);
	
	/* 初回のﾌｨｰﾄﾞﾊﾞｯｸ値通知対策(再生が始まるまでは制御目標通りであることにする) */
	audio_buffer_count_fixed = (AUDIO_BUFFER_SIZE / 4);
#if (EQ_MODE != EQ_MODE_OFF)
	eq_init();
#endif
	
	/* @memo ﾊﾞｯﾌｧの初期化は行わない。実際の再生開始までに呼出元でﾃﾞｰﾀ補充を行うこと */
}

/*@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@*/
/*
	ﾊﾞｯﾌｧへﾃﾞｰﾀｽﾄﾘｰﾑを供給する
	
	@warning sizeは必ずﾌﾚｰﾑ長の整数倍。0は不可
*/
void audio_buffer_feed(unsigned char *buf, int size){
	
	/* -O0向けの最適化 */
	struct {
		unsigned short val[4];
	} *in_data = (void *)buf, *out_data = (void *)(&(audio_buffer.data[audio_buffer.wp]));
	
	int i = audio_buffer.wp;
	const int size_org = size;
	
	/* 基本的に割禁にしない運用なので、先に更新 */
	/* 但し、本来この部分だけはｶﾞｰﾄﾞすべき */
	//__disable_irq();
	audio_buffer.wp = (audio_buffer.wp + size_org) % sizeof(audio_buffer.data);
	//__enable_irq();
	
	/* 上位ﾜｰﾄﾞ→下位ﾜｰﾄﾞの順に送信する必要があるので、ここで並べ替え */
	/* (でもﾜｰﾄﾞ内はﾘﾄﾙｴﾝﾃﾞｨｱﾝ…) */
	do{
#if (EQ_MODE == EQ_MODE_OFF)
		/* I2S L */
		out_data->val[0] = in_data->val[1];
		out_data->val[1] = in_data->val[0];
		/* I2S R */
		out_data->val[2] = in_data->val[3];
		out_data->val[3] = in_data->val[2];
#else
		int32_t l = (int32_t)(((uint32_t)in_data->val[1] << 16) | in_data->val[0]);
		int32_t r = (int32_t)(((uint32_t)in_data->val[3] << 16) | in_data->val[2]);

		l = eq_process(eq_l, l);
		r = eq_process(eq_r, r);

		/* I2S L: high word first */
		out_data->val[0] = (uint16_t)((uint32_t)l >> 16);
		out_data->val[1] = (uint16_t)((uint32_t)l);
		/* I2S R */
		out_data->val[2] = (uint16_t)((uint32_t)r >> 16);
		out_data->val[3] = (uint16_t)((uint32_t)r);
#endif
		
		in_data++;
		
		i += 8;
		i = (i >= sizeof(audio_buffer.data)) ? 0 : i;
		
		out_data = (void *)(&(audio_buffer.data[i]));
		
	}while (size -= 8);
	
	audio_buffer_count += size_org;
	audio_buffer_count_valid = audio_buffer_count;	/* 同期 */
}

/*@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@*/
/*
	ﾊﾞｯﾌｧﾌｨﾙ
	
	valid != 0: 供給ﾃﾞｰﾀを有効ﾃﾞｰﾀにしたい場合
	valid == 0: 供給ﾃﾞｰﾀを無効ﾃﾞｰﾀにしたい場合(DMA停止時の零ﾌｨﾙを想定)
	
	@warning sizeは必ずﾌﾚｰﾑ長の整数倍。0は不可
*/
void audio_buffer_fill(int val, int size, int valid){
	
	int i = audio_buffer.wp;
	const int size_org = size;
	
	/* 基本的に割禁にしない運用なので、先に更新 */
	/* 但し、本来この部分だけはｶﾞｰﾄﾞすべき */
	//__disable_irq();
	audio_buffer.wp = (audio_buffer.wp + size_org) % sizeof(audio_buffer.data);
	//__enable_irq();
	
	do{
		*((int *)(&(audio_buffer.data[i]))) = val;
		i += 4;
		i = (i >= sizeof(audio_buffer.data)) ? 0 : i;
	}while (size -= 4);
	
	audio_buffer_count += size_org;
	if (valid) audio_buffer_count_valid = audio_buffer_count;	/* 同期 */
}

/*@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@*/
/*
	次のﾊﾞｯﾌｧ中間地点 / 末尾までを指定ﾊﾞｲﾄで埋める
	
	中間 / 末尾 == DMA転送で割り込みがかかる地点を想定。
	
	@warning sizeは必ずﾌﾚｰﾑ長の整数倍。0は不可
	@warning audio_buffer_count_validは更新されない(DMA停止時のｵｰﾊﾞﾗﾝｴﾘｱを予めﾌｨﾙしておく目的の為)
*/
void audio_buffer_fill_next_ip(int val){
	
	int i = audio_buffer.wp;
	int size = (sizeof(audio_buffer.data) / 2) - (i % (sizeof(audio_buffer.data) / 2));
	const int size_org = size;
	
	/* 基本的に割禁にしない運用なので、先に更新 */
	/* 但し、本来この部分だけはｶﾞｰﾄﾞすべき */
	//__disable_irq();
	audio_buffer.wp = (audio_buffer.wp + size_org) % sizeof(audio_buffer.data);
	//__enable_irq();
	
	/* 次の割り込み地点までﾌｨﾙ */
	do{
		*((int *)(&(audio_buffer.data[i]))) = val;
		i += 4;
		i = (i >= sizeof(audio_buffer.data)) ? 0 : i;
	}while (size -= 4);
	
	audio_buffer_count += size_org;
}

/*@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@*/
/*
	ﾊﾞｯﾌｧの先頭ｱﾄﾞﾚｽ取得
*/
unsigned char *audio_buffer_getptr(){
	return &(audio_buffer.data[0]);
}

/*@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@*/
/*
	ﾊﾞｯﾌｧからの読み出し(指定ｵﾌｾｯﾄのﾊﾞｯﾌｧｱﾄﾞﾚｽ取得)
	
	@memo ﾌｨｰﾄﾞﾊﾞｯｸ値算出用の残量計が更新される
	      このため、DMA転送する場合も転送完了ｲﾍﾞﾝﾄ毎に呼び出すこと
	      また、読みだすｻｲｽﾞを必ず申告すること
	
	@warning sizeは必ずﾌﾚｰﾑ長の整数倍。0許容
*/
unsigned char *audio_buffer_read(int ofs, int size){
	
	if (ofs < sizeof(audio_buffer.data)){
		
		audio_buffer_count -= size;
		audio_buffer_count_valid -= size;
		
#if 0
		/* ｵｰﾊﾞ / ｱﾝﾀﾞｰﾌﾛｰ対策 */
		/* つまり、現象が発生した際は、周囲のﾃﾞｰﾀを破壊してでも早期収束を図る */
		while (audio_buffer_count >= (signed int)AUDIO_BUFFER_SIZE){
//			HAL_GPIO_TogglePin(GPIOD, LD5_Pin);
			audio_buffer_count -= AUDIO_BUFFER_SIZE;
		}
		while (audio_buffer_count < 0){
//			HAL_GPIO_TogglePin(GPIOD, LD4_Pin);
			audio_buffer_count += AUDIO_BUFFER_SIZE;
		}
#endif
		/* ｵｰﾊﾞ / ｱﾝﾀﾞｰﾗﾝﾁｪｯｸ */
		HAL_GPIO_WritePin(GPIOD, LD4_Pin, (audio_buffer_count < 0) ? GPIO_PIN_SET : GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOD, LD5_Pin, (audio_buffer_count > (signed int)AUDIO_BUFFER_SIZE) ? GPIO_PIN_SET : GPIO_PIN_RESET);
		
		audio_buffer_count_fixed = audio_buffer_count;
		
		/* ﾊﾞｯﾌｧ残数を表示(ﾌｨｰﾄﾞﾊﾞｯｸによって左右均等に点くような状態が好ましい) */
//		HAL_GPIO_WritePin(GPIOD, LD4_Pin, (audio_buffer_count_fixed <= (signed int)(AUDIO_BUFFER_SIZE / 4)) ? GPIO_PIN_SET : GPIO_PIN_RESET);
//		HAL_GPIO_WritePin(GPIOD, LD5_Pin, (audio_buffer_count_fixed >= (signed int)(AUDIO_BUFFER_SIZE / 4)) ? GPIO_PIN_SET : GPIO_PIN_RESET);
		
		return &(audio_buffer.data[ofs]);
		
	}else{
		return (void *)0;
	}
}

/*@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@*/
/*
	ﾊﾞｯﾌｧへの供給ｻｲｽﾞを調整する為のﾌｨｰﾄﾞﾊﾞｯｸ値を取得
	
	値は、制御目標となるﾊﾞｯﾌｧ残量との差[ｻﾝﾌﾟﾙ数単位]。
	目標値 > 残 → 正(つまり、ﾊﾞｯﾌｧ残を増やしたい)
	目標値 < 残 → 負(つまり、ﾊﾞｯﾌｧ残を減らしたい)
*/
signed int audio_buffer_getfeedback(){
	
	/* @memo 制御目標は、DMA半ﾊﾞｯﾌｧ転送割り込み後にﾃﾞｰﾀ残数が1/4ﾊﾞｯﾌｧ分ある状態 */
	/* (buf残 > (AUDIO_BUFFER_SIZE / 4)) → 要求量が小さくなる方向 */
	/* (buf残 < (AUDIO_BUFFER_SIZE / 4)) → 要求量が大きくなる方向 */
	return ((signed int)(AUDIO_BUFFER_SIZE / 4) - audio_buffer_count_fixed) / AUDIO_BUFFER_FRAMESIZE;
}
