#include "zlib.h"
#include "string.h"
#include "malloc.h"
#include "ColdevCompress.h"
#include "stdio.h"

ColdevCompressData::ColdevCompressData()
{
	pCompressedData =  NULL;
	pUncompressedData =  NULL;
}


ColdevCompressData::~ColdevCompressData()
{
	if (pCompressedData) { free( pCompressedData);   pCompressedData = NULL; 	}

	if (pUncompressedData) { free(pUncompressedData);  pUncompressedData =  NULL; }
}


int ColdevCompressData::cd_compressBytes(const unsigned char *pData, unsigned long nDataSize, int *destbuffer, int *destlength)
{
 
	*destlength = 0;


	unsigned long nCompressedDataSize = nDataSize * 2;

//	if (pCompressedData) { free(pCompressedData);   pCompressedData = NULL; }

	pCompressedData = (unsigned char *)  malloc(nCompressedDataSize);



   // if (!pCompressedData) return Z_BUF_ERROR; 



	int nResult = compress2(pCompressedData, &nCompressedDataSize, pData, nDataSize, 9);

	if (nResult == Z_OK)
	{
//		printf("Compressed size: %d\n", nCompressedDataSize);

		*destbuffer = (int)pCompressedData;
		*destlength = nCompressedDataSize;
	}

	return nResult;

}

 

int ColdevCompressData::cd_uncompressBytes(unsigned char * pCompressedData, unsigned long nCompressedDataSize, int *destbuffer, int *destlength)
{
	 
	unsigned long nDataSize = *destlength;

//	if (pUncompressedData) { free(pUncompressedData);  pUncompressedData = NULL; }

	pUncompressedData = (unsigned char *)malloc(nDataSize);
		

  //  if (!pUncompressedData) return Z_BUF_ERROR;


	int	nResult = uncompress(pUncompressedData, &nDataSize, pCompressedData, nCompressedDataSize);

	if (nResult == Z_OK)
	{
	//	printf("Uncompressed size: %d\n", nDataSize);

		*destbuffer = (int)pUncompressedData;
		*destlength = nDataSize;
	}

//	printf("======================  %d \n", nResult);

	return nResult;

}