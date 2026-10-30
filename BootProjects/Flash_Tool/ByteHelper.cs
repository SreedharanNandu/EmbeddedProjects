using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Byte_Help
{
    public static class ByteHelper
    {
        /// <summary>
        /// Combines any number of byte arrays into a single, large array.
        /// The sequence of the data is left unchanged, and it is as if later
        /// arrays are appended to the end of preceding arrays.
        /// </summary>
        /// <param name="arrays"></param>
        /// <returns></returns>
        public static byte[] Combine(params byte[][] arrays)
        {
            byte[] result = new byte[arrays.Sum(x => x.Length)];
            int offset = 0;
            foreach (byte[] array in arrays)
            {
                Buffer.BlockCopy(array, 0, result, offset, array.Length);
                offset += array.Length;
            }
            return result;
        }

      
        public static string BytesToString(byte[] data)
        {
            if (data == null)
            {
                return "{ null }";
            }

            StringBuilder output = new StringBuilder();
            //output.Append("{ ");
            for (int index = 0; index < data.Length - 1; index++)
            {
                //Print all but the last byte so commas appear correctly.
                output.AppendFormat("{0:X2}:", data[index]);
            }
            if (data.Length > 0)
            {
                //Print last byte (only if there is at least one byte in the array).
                output.AppendFormat("{0:X2}", data[data.Length - 1]);
            }
            //output.Append(" }");
            return output.ToString();
        }
        public static string BytesToASCIIString(byte[] data)
        {
            if (data == null)
            {
                return "{ null }";
            }

            StringBuilder output = new StringBuilder();
            //output.Append("{ ");
            for (int index = 0; index < data.Length - 1; index++)
            {
                //Print all but the last byte so commas appear correctly.
                output.AppendFormat("{0:X2}:", data[index]);
            }
            if (data.Length > 0)
            {
                //Print last byte (only if there is at least one byte in the array).
                output.AppendFormat("{0:X2}", data[data.Length - 1]);
            }
            //output.Append(" }");
            return output.ToString();
        }
        public static string BytesToBinaryString(byte[] data)
        {
            if (data == null)
            {
                return "{ null }";
            }

            StringBuilder output = new StringBuilder();
            output.Append("{ ");
            for (int index = 0; index < data.Length - 1; index++)
            {
                //Print all but the last byte so commas appear correctly.
                output.AppendFormat("{0}, ", Convert.ToString(data[index], 2).PadLeft(8, '0'));
            }
            if (data.Length > 0)
            {
                //Print last byte (only if there is at least one byte in the array).
                output.AppendFormat("{0}", Convert.ToString(data[data.Length - 1], 2).PadLeft(8, '0'));
            }
            output.Append(" }");
            return output.ToString();
        }
    }
}
