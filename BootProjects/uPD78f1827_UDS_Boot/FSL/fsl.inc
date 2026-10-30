;=================================================================================================
; Project      = Flash self-programming library, MF2 single voltage flash
; Module       = fsl.inc 
; Device:      = 78K0R/Ix3/Kx3-L/Kx3-A/Kx3-C/Lx3
; Version      = V1.20
; Date         = 17.02.2009 11:06:06
;=================================================================================================
;                                  COPYRIGHT
;=================================================================================================
; Copyright (c) 2008, 2009 by NEC Electronics (Europe) GmbH,
;               a company of the NEC Electronics Corporation
;=================================================================================================
; Purpose:
;              implementation module of low-level flash memory access
;              CAUTION: Please comment out unused function prototypes 
;                       for code size reduction.
;
;=================================================================================================
;
; Warranty Disclaimer
;
; Because the Product(s) is licensed free of charge, there is no warranty of any kind
; whatsoever and expressly disclaimed and excluded by NEC, either expressed or implied,
; including but not limited to those for non-infringement of intellectual property,
; merchantability and/or fitness for the particular purpose. NEC shall not have any obligation
; to maintain, service or provide bug fixes for the supplied Product(s) and/or the Application.
;
; Each User is solely responsible for determining the appropriateness of using the Product(s)
; and assumes all risks associated with its exercise of rights under this Agreement,
; including, but not limited to the risks and costs of program errors, compliance with
; applicable laws, damage to or loss of data, programs or equipment, and unavailability or
; interruption of operations.
;
; Limitation of Liability
;
; In no event shall NEC be liable to the User for any incidental, consequential, indirect,
; or punitive damage (including but not limited to lost profits) regardless of whether
; such liability is based on breach of contract, tort, strict liability, breach of warranties,
; failure of essential purpose or otherwise and even if advised of the possibility of
; such damages. NEC shall not be liable for any services or products provided by third party
; vendors, developers or consultants identified or referred to the User by NEC in connection
; with the Product(s) and/or the Application.
;
;=================================================================================================
;
; Environment  = PM plus(V6.30)
;                RA78K0R(V1.20)
;                CC78K0R(V2.00)
;
;=================================================================================================


$IF (__FSL_INC_INCLUDED)

$ELSE
$SET (__FSL_INC_INCLUDED)




;==============================================================================================
; constant definitions                                                                         
;==============================================================================================

; status code definitions returned by the FSL functions

FSL_OK                  EQU  00H
FSL_ERR_FLMD0           EQU  01H
FSL_ERR_PARAMETER       EQU  05H
FSL_ERR_PROTECTION      EQU  010H
FSL_ERR_ERASE           EQU  01AH
FSL_ERR_BLANKCHECK      EQU  01BH
FSL_ERR_IVERIFY         EQU  01BH
FSL_ERR_WRITE           EQU  01CH
FSL_ERR_EEP_IVERIFY     EQU  01DH
FSL_ERR_EEP_BLANKCHECK  EQU  01EH
FSL_ERR_INTERRUPTION    EQU  01FH



;----------------------------------------------------------------------------------------------
; Block type:   FSL command function                                                           
;----------------------------------------------------------------------------------------------
; Purpose:      initialization of selfprogramming environment                                  
;               After initialization:                                                          
;               - the pointer to the data-buffer is stored                
;               - all timing data are re-calculated according to the used system clock         
;                                                                                              
;               CAUTION:                                                                       
;               The FSL_Init function is interruptible. Please use the           
;               FSL_Init_cont to recall it as long return status is 0x1F.        
;                                                                                              
; Input:        AX - address of data buffer of N...256 bytes                   
;                                 (used for data exchange between firmware and application)    
; Output:       -                                                                              
; Changed:      AX, data_buffer[0]
; Returned:     C, status code                                                               
;                    = 0x00(FSL_OK), normal and means initialization OK                        
;                    = 0x1F(FSL_ERR_INTERRUPTION), initialization interrupted by user interrupt
;----------------------------------------------------------------------------------------------
EXTRN _FSL_Init
EXTRN _FSL_Init_cont


;----------------------------------------------------------------------------------------------
; Block type:   FSL command function                                                           
;----------------------------------------------------------------------------------------------
; Purpose:      checks the voltage level (high or low) at FLMD0 pin                            
; Input:        -                                                                              
; Output:       -                                                                              
; Changed:      -
; Returned:     C, status code                                                           
;                    = 0x00(FSL_OK), normal and means FLMD0=HIGH                               
;                    = 0x01(FSL_ERR_FLMD0), error, FLMD0=LOW                                   
;----------------------------------------------------------------------------------------------
EXTRN _FSL_ModeCheck


;----------------------------------------------------------------------------------------------
; Block type:   FSL command function                                                           
;----------------------------------------------------------------------------------------------
; Purpose:      checks if specified block is blank                                             
; Input:        AX - block number                                    
; Output:       -                                                                              
; Changed:      AX, ES
; Returned:     C, status code                                                           
;                    = 0x00(FSL_OK), normal and means "block is blank"                         
;                    = 0x05(FSL_ERR_PARAMETER), parameter error                                
;                    = 0x1B(FSL_ERR_BLANKCHECK), blank-check error, means "block not blank"    
;                    = 0x1F(FSL_ERR_INTERRUPTION), blank-check interrupted by user interrupt   
;----------------------------------------------------------------------------------------------
EXTRN _FSL_BlankCheck


;----------------------------------------------------------------------------------------------
;  Block type:   FSL command function                                                          
;----------------------------------------------------------------------------------------------
;  Purpose:      erase specified block                                                         
;  Input:        AX - block number                                      
;  Output:       -                                                                             
;  Changed:      AX, ES
;  Returned:     C, status code                                                          
;                     = 0x00(FSL_OK), normal and means "block erased successfully"             
;                     = 0x05(FSL_ERR_PARAMETER), parameter error                               
;                     = 0x10(FSL_ERR_PROTECTION), tried to erase protected area                
;                     = 0x1A(FSL_ERR_ERASE), erase error, retry up to max. 255 times           
;                     = 0x1F(FSL_ERR_INTERRUPTION), erasing interrupted by user interrupt      
;----------------------------------------------------------------------------------------------
EXTRN _FSL_Erase


;----------------------------------------------------------------------------------------------
; Block type:   FSL command function                                                           
;----------------------------------------------------------------------------------------------
; Purpose:      performs internal verify on specified block                                    
; Input:        AX - block number                                   
; Output:       -                                                                              
; Changed:      AX, ES
; Returned:     C, status code                                                           
;                    = 0x00(FSL_OK), normal and means "block is verified"                      
;                    = 0x05(FSL_ERR_PARAMETER), parameter error                                
;                    = 0x1B(FSL_ERR_IVERIFY), internal verify error                            
;                    = 0x1F(FSL_ERR_INTERRUPTION), verify interrupted by user interrupt        
;----------------------------------------------------------------------------------------------
EXTRN _FSL_IVerify


;----------------------------------------------------------------------------------------------
; Block type:   FSL command function                                                           
;----------------------------------------------------------------------------------------------
; Purpose:      writes N words from the data buffer into flash                                 
; Input:        AX = HIGH(address)        - starting flash address the data has to be written              
;               BC = LOW(address)           See Condition  2) please.                                      
;               over stack  = word_count  - number of words (on word 4 bytes) has to be written                
; Output:       -                                                                              
; Condition:   1) (address MOD 4 == 0)                                                   
;              2) most significant byte (MSB) of  address has to be 0x00.                
;                 Means: 0x00abcdef 24 bit flash address allowed                               
;              3) (word_count <= sizeof(data buffer))  NOT CHECKED BY LIBRARY !!!!!            
; Changed:     AX, BC, ES                                                                      
; Returned:    C, status code                                                                  
;                   = 0x00(FSL_OK), normal                                                     
;                   = 0x05(FSL_ERR_PARAMETER), parameter error                                 
;                   = 0x10(FSL_ERR_PROTECTION), protection error                               
;                   = 0x1C(FSL_ERR_WRITE), write error                                         
;                   = 0x1F(FSL_ERR_INTERRUPTION), write interrupted by user interrupt          
;----------------------------------------------------------------------------------------------
EXTRN _FSL_Write


;----------------------------------------------------------------------------------------------
; Block type:   FSL command function                                                           
;----------------------------------------------------------------------------------------------
; Purpose:      writes N words from the data buffer into flash                                 
;               Before "writing" a N-word blankcheck is performed.                             
;               After "writing" a N-Word internal verify is performed.                         
; Input:        AX = HIGH(address)        - starting flash address the data has to be written              
;               BC = LOW(address)           See Condition  2) please.                                      
;               over stack  = word_count  - number of words (on word 4 bytes) has to be written                
; Output:       -                                                                              
; Condition:   1) (address MOD 4 == 0)                                                   
;              2) most significant byte (MSB) of  address has to be 0x00.                
;                 Means: 0x00abcdef 24 bit flash address allowed                               
;              3) (word_count <= sizeof(data buffer))  NOT CHECKED BY LIBRARY !!!!!            
; Changed:     AX, BC, ES                                                                      
; Returned:    C, status code                                                                  
;                   = 0x00(FSL_OK), normal                                                     
;                   = 0x05(FSL_ERR_PARAMETER), parameter error                                 
;                   = 0x10(FSL_ERR_PROTECTION), protection error                               
;                   = 0x1C(FSL_ERR_WRITE), write error                                         
;                   = 0x1D(FSL_ERR_EEP_IVERIFY), verify error                                  
;                   = 0x1E(FSL_ERR_EEP_BLANKCHECK), blankcheck error                           
;                   = 0x1F(FSL_ERR_INTERRUPTION), write interrupted by user interrupt          
;----------------------------------------------------------------------------------------------
EXTRN _FSL_EEPROMWrite


;----------------------------------------------------------------------------------------------
; Block type:   FSL command function                                                           
;----------------------------------------------------------------------------------------------
; Purpose:      reads the security information                                                 
; Input:        AX - destination address of security info                                         
;                                                                                              
;               Format of the security info:                                                   
;               bit_0 = 0 -> chip erase command disabled, otherwise enabled                    
;               bit_1 = 0 -> block erase command disabled, otherwise enabled                   
;               bit_2 = 0 -> write command disabled, otherwise enabled                         
;               bit_4 = 0 -> boot-area re-programming disabled, otherwise enabled              
;               bit_8...bit_15 = 03H -> last block of the boot-area                            
;               other bits = 1                                                                 
; Output:       -                                                                              
; Changed:      AX, BC and data_buffer[0], data_buffer[1]
; Returned:     C, status code                                                           
;                  = 0x00(FSL_OK), normal                                                      
;                  = 0x05(FSL_ERR_PARAMETER), parameter error 
;               [destination address], security information                                  
;----------------------------------------------------------------------------------------------
EXTRN _FSL_GetSecurityFlags


;----------------------------------------------------------------------------------------------
; Block type:   FSL command function                                                           
;----------------------------------------------------------------------------------------------
; Purpose:      read the boot flag i                                               
;               The value of the boot info is 0x00 for cluster 0 and 0x01 for cluster 1.            
; Input:        AX - destination address of the bootflag info                                                
;                    ATTENTION: the size of the destination varibale must be min. size 1 byte      
; Output:       -                                                                              
; Changed:      data_buffer[0]                                                     
; Returned:     C, status code                                                           
;                  = 0x00(FSL_OK), normal                                                      
;                  = 0x05(FSL_ERR_PARAMETER), parameter error 
;               [destination address], active bootcluster information                                  
;----------------------------------------------------------------------------------------------
EXTRN _FSL_GetActiveBootCluster


;----------------------------------------------------------------------------------------------
; Block type:   FSL command function                                                           
;----------------------------------------------------------------------------------------------
; Purpose:     puts the last address of the specified block into *destination_pu32             
; Input:       AX - destination where the last-block-address should be stored
;                   ATTENTION: the size of the destination varibale must be min. size 4 byte
;                                                                              
;              over stack - block number
; Changed:     AX, data_buffer[0] to data_buffer[3]                                                                               
; Returned:    C, status code                                                            
;                   = 0x00(FSL_OK), normal                                                     
;                   = 0x05(FSL_ERR_PARAMETER), parameter error                                 
;----------------------------------------------------------------------------------------------
EXTRN _FSL_GetBlockEndAddr


;----------------------------------------------------------------------------------------------
; Block type:   FSL command function                                                           
;----------------------------------------------------------------------------------------------
; Purpose:     puts the information about the protected flash area into the function parameter 
; Input:       AX - destination where the FSW start block should be stored      
;              over stack  - destination where the FSW end block should be stored 
;              ATTENTION: the size of the destination varibales must be min. size 2 byte       
; Changed:     data_buffer[0] to data_buffer[3]                                                                               
; Returned:    C, status code                                                            
;                   = 0x00(FSL_OK), normal                                                     
;                   = 0x05(FSL_ERR_PARAMETER), parameter error                                 
;----------------------------------------------------------------------------------------------
EXTRN _FSL_GetFlashShieldWindow


;----------------------------------------------------------------------------------------------
; Block type:   FSL command function                                                           
;----------------------------------------------------------------------------------------------
; Purpose:     Inverts the security boot flag.                                
;                                                                                          
;              CAUTION !!!!                                                   
;              After reset the boot clusters will be switched                 
;              regarding this security boot flag      
;                        
; Input:       -                                                                               
; Output:      -                                                                               
; Changed:     data_buffer[0] to data_buffer[51] 
; Returned:    C, status code                                                            
;                   = 0x00(FSL_OK), normal                                                     
;                   = 0x05(FSL_ERR_PARAMETER), parameter error                                 
;                   = 0x10(FSL_ERR_PROTECTION), protection error                               
;                   = 0x1A(FSL_ERR_ERASE), erase error                                         
;                   = 0x1B(FSL_ERR_IVERIFY), internal verify error                             
;                   = 0x1F(FSL_ERR_INTERRUPTION), write interrupted by user interrupt          
;----------------------------------------------------------------------------------------------
EXTRN _FSL_InvertBootFlag


;----------------------------------------------------------------------------------------------
; Block type:   FSL command function                                                           
;----------------------------------------------------------------------------------------------
; Purpose:     defines a new Flash-Shield-Window area inside the flash memory                  
; Input:       AX - starting block of the Flash-Shield-Window (FSW)               
;              over stack - end block of the flash-Shield-Window (FSW)                 
; Changed:     data_buffer[0] to data_buffer[4]                                                                               
; Returned:    C, status code                                                            
;                    = 0x00(FSL_OK), normal                                                    
;                    = 0x05(FSL_ERR_PARAMETER), parameter error                                
;                    = 0x10(FSL_ERR_PROTECTION), protection error                              
;                    = 0x1A(FSL_ERR_ERASE), erase error                                        
;                    = 0x1B(FSL_ERR_IVERIFY), internal verify error                            
;                    = 0x1F(FSL_ERR_INTERRUPTION), write interrupted by user interrupt         
;----------------------------------------------------------------------------------------------
EXTRN _FSL_SetFlashShieldWindow


;----------------------------------------------------------------------------------------------
; Block type:   FSL command function                                                           
;----------------------------------------------------------------------------------------------
; Purpose:      sets specified security flag by dedicated command-function.                    
;                                                                                              
;               There are following security levels:                                           
;               a) chip-erase protection   (cannot be reset by programmer !!!)                 
;               b) block-erase protection  (can be reset by chip-erase on programmer)          
;               c) write protection        (can be reset by chip-erase on programmer)          
;               d) boot-cluster protection (cannot be reset by programmer !!!)                 
;                                                                                              
;               CAUTION !!!!                                                                             
;               Each security flag can be written by the application only once              
;                                                                                              
; Input:        -                                                                              
; Output:       -                                                                              
; Changed:     data_buffer[0] to data_buffer[4]
; Returned:     C, status code                                                           
;                    = 0x00(FSL_OK), normal                                                    
;                    = 0x05(FSL_ERR_PARAMETER), parameter error                                
;                    = 0x10(FSL_ERR_PROTECTION), protection error                              
;                    = 0x1A(FSL_ERR_ERASE), erase error                                        
;                    = 0x1B(FSL_ERR_IVERIFY), internal verify error                            
;                    = 0x1F(FSL_ERR_INTERRUPTION), write interrupted by user interrupt         
;----------------------------------------------------------------------------------------------
EXTRN _FSL_SetChipEraseProtectFlag
EXTRN _FSL_SetBlockEraseProtectFlag
EXTRN _FSL_SetWriteProtectFlag
EXTRN _FSL_SetBootClusterProtectFlag


;----------------------------------------------------------------------------------------------
; Block type:   FSL command function                                                           
;----------------------------------------------------------------------------------------------
; Purpose:     Swap physically boot cluster 0 and boot cluster 1 without setting             
;              the security flag. After swap the library jumps to the address of the         
;              reset vector.                                                                 
;              CAUTION:  After reset the boot cluster will be switched                       
;                        regarding the security flag.                                        
;                                                                                            
; Input:       -                                                                               
; Output:      -                                                                               
; Changed:     AX, BC, ES, CS and data_buffer[0] to data_buffer[51]
; Returned:    -      
;----------------------------------------------------------------------------------------------
EXTRN _FSL_SwapBootCluster


;----------------------------------------------------------------------------------------------
; Block type:   FSL command function                                                           
;----------------------------------------------------------------------------------------------
; Purpose:     Inverts the security boot flag and swaps physically 
;							 boot cluster 0 and boot cluster 1.                                
;                                                                                          
;              CAUTION !!!!                                                   
;              After reset the boot clusters will be switched                 
;              regarding this security boot flag      
;                        
; Input:       -                                                                               
; Output:      -                                                                               
; Changed:     data_buffer[0] to data_buffer[4]
; Returned:    C, status code                                                            
;                   = 0x00(FSL_OK), normal                                                     
;                   = 0x05(FSL_ERR_PARAMETER), parameter error                                 
;                   = 0x10(FSL_ERR_PROTECTION), protection error                               
;                   = 0x1A(FSL_ERR_ERASE), erase error                                         
;                   = 0x1B(FSL_ERR_IVERIFY), internal verify error                             
;                   = 0x1F(FSL_ERR_INTERRUPTION), write interrupted by user interrupt          
;----------------------------------------------------------------------------------------------
EXTRN _FSL_SwapActiveBootCluster


;----------------------------------------------------------------------------------------------
; Block type:   FSL command function                                                           
;----------------------------------------------------------------------------------------------
; Purpose:     Forces a hardware reset via illegal instruction                                         
;                                                                                            
; Input:       -                                                                               
; Output:      - 
; Changed:     -                                                                              
; Returned:    -      
;----------------------------------------------------------------------------------------------
EXTRN _FSL_ForceReset


;----------------------------------------------------------------------------------------------
; Block type:   FSL function                                                                   
;----------------------------------------------------------------------------------------------
; Purpose:     This function forces to leave the running FSL_XXX function as 
;              fast as possible. It must be called only inside ISRs.
; Input:       -    
; Changed:     -                                                                               
; Returned:    -                                                                               
;----------------------------------------------------------------------------------------------
EXTRN _FSL_SetInterruptMode






$ENDIF
