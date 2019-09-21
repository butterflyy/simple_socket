#ifndef __BASE64_H__
#define __BASE64_H__

#include <string>

typedef std::string bytes;

class base64 
{
public:

	static bytes encode(const bytes& source);

	static bool decode(const bytes& source, bytes& target);

private:
		base64();
		~base64();

		static char Base64Code[64];

		static int Base64Index(char base64Char)
		{
			int index=0;
			for(;index<64;index++)
				if (base64Char==Base64Code[index])
					return index;
			return -1;
		}
};

char base64::Base64Code[64] = 
		{
			'A','B','C','D','E','F','G','H',
			'I','J','K','L','M','N','O','P',
			'Q','R','S','T','U','V','W','X',
			'Y','Z','a','b','c','d','e','f',
			'g','h','i','j','k','l','m','n',
			'o','p','q','r','s','t','u','v',
			'w','x','y','z','0','1','2','3',
			'4','5','6','7','8','9','+','/'
		};


bytes base64::encode(const bytes& source)
{
	int num = source.length() / 3;
	int rest = source.length() % 3;
	int i=0;
	unsigned char byte1,byte2,byte3;
	char chs[4];

	bytes result;
	result.resize((num + 1) * 4 + 1, 0);

	for (;i<num;i++)
	{
		byte1 = source[i*3];
		byte2 = source[i*3+1];
		byte3 = source[i*3+2];

		chs[0] = Base64Code[(byte1>>2)];
		chs[1] = Base64Code[((byte1 & 0x3)<<4)+(byte2>>4)];
		chs[2] = Base64Code[((byte2 & 0xf)<<2)+(byte3>>6)];
		chs[3] = Base64Code[(byte3 & 0x3f)];
		memcpy((void*)(result.c_str() + i * 4), chs, sizeof(chs));
	}
	if(rest==1)
	{
		byte1 = source[i*3];
		chs[0] = Base64Code[byte1>>2];
		chs[1] = Base64Code[(byte1&0x3)<<4];
		chs[2] = '=';
		chs[3] = '=';
		memcpy((void*)(result.c_str() + i * 4), chs, sizeof(chs));
	}
	if(rest==2)
	{
		byte1 = source[i*3];
		byte2 = source[i*3+1];
		chs[0] = Base64Code[byte1>>2];
		chs[1] = Base64Code[((byte1&0x3)<<4)+(byte2>>4)];
		chs[2] = Base64Code[(unsigned char)((source[i*3+1]&0xf)<<2)];
		chs[3] = '=';
		memcpy((void*)(result.c_str() + i * 4), chs, sizeof(chs));
	}

	return bytes(result.c_str());
}

bool base64::decode(const bytes& source, bytes& target)
{
	size_t sourceLength = source.length();

	bytes temp;

	temp.resize(sourceLength, 0);

	if(sourceLength < 4 || sourceLength % 4)
		return false;

	int eNum = 0;
	int num = 0;
	if(source[sourceLength-1] == '=')
		eNum++;
	if(source[sourceLength-2] == '=')
		eNum++;

	num = sourceLength/4;
	if(eNum!=0)
		num--;

	int i = 0, destIndex = 0;
	unsigned char byte1 = 0,byte2 = 0,byte3 = 0,byte4 = 0;
	for (;i<num;i++)
	{
		byte1 = Base64Index(source[i*4]);
		byte2 = Base64Index(source[i*4+1]);
		byte3 = Base64Index(source[i*4+2]);
		byte4 = Base64Index(source[i*4+3]);

		temp[destIndex++] = (byte1<<2)+(byte2>>4);
		temp[destIndex++] = (byte2<<4)+(byte3>>2);
		temp[destIndex++] = (byte3<<6)+byte4;
	}
	if(eNum==1)
	{
		byte1  = Base64Index(source[i*4]);
		byte2 = Base64Index(source[i*4+1]);
		byte3 = Base64Index(source[i*4+2]);
		temp[destIndex++] = (byte1<<2)+(byte2>>4);
		temp[destIndex++] = ((byte2&0xf)<<4)+(byte3>>2);
	}
	if(eNum==2)
	{
		byte1  = Base64Index(source[i*4]);
		byte2 = Base64Index(source[i*4+1]);
		temp[destIndex++] = (byte1<<2)+(byte2>>4);
	}


	target.resize(destIndex, 0);

	memcpy((void*)target.c_str(), temp.c_str(), target.length());

	return true;
}

#endif //!__BASE64_H__