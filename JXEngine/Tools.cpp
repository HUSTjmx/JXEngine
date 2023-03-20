#include "Tools.h"
#include <iostream>
std::string StringTool::DelLineFromFile(std::ifstream& file, unsigned int num)
{
	char temp[1024] = { 0 };
	std::string result;
	int c_num = 10;
	while (file.getline(temp, sizeof(temp)))
	{
		if (c_num == num)
		{
			result += "";
		}
		else {
			result += temp;
			result += "";
		}
		c_num++;
	}
	return result;
}

std::string StringTool::Convert(float Num) const noexcept
{
	std::ostringstream oss;
	oss << Num;
	std::string str(oss.str());
	return str;
}

void SortPosByDistance(std::vector<glm::vec3>& v, glm::vec3 c_pos)
{
	auto func = [c_pos](glm::vec3 a, glm::vec3 b)
	{
		return glm::length(c_pos - a) > glm::length(c_pos - b);
	};

	std::sort(v.begin(), v.end(), func);
	//std::sort(v.begin(), v.end(),)
}

glm::mat4 MatTool::RemoveTranslation(glm::mat4 a)
{
	return glm::mat4(glm::mat3(a));
}

void SphereCreator::Create_ICO(std::vector<float>& v_d, std::vector<unsigned int>& indexs, std::vector<unsigned int>& offsets, int recursionLevel)
{
	m_cache.clear();
	vertexs.clear();

	index = 0;

	offsets.push_back(3);
	offsets.push_back(3);
	offsets.push_back(2);

	auto t = (1.0f + std::sqrt(5.0f)) / 2.0f;

	AddVertex(glm::vec3(-1, t, 0));
	AddVertex(glm::vec3(1, t, 0));
	AddVertex(glm::vec3(-1, -t, 0));
	AddVertex(glm::vec3(1, -t, 0));

	AddVertex(glm::vec3(0, -1, t));
	AddVertex(glm::vec3(0, 1, t));
	AddVertex(glm::vec3(0, -1, -t));
	AddVertex(glm::vec3(0, 1, -t));
	

	AddVertex(glm::vec3(t, 0, -1));
	AddVertex(glm::vec3(t, 0, 1));
	AddVertex(glm::vec3(-t, 0, -1));
	AddVertex(glm::vec3(-t, 0, 1));

	std::vector<face> faces;

	// 1
	faces.push_back(face(0, 11, 5));
	faces.push_back(face(0, 5, 1));
	faces.push_back(face(0, 1, 7));
	faces.push_back(face(0, 7, 10));
	faces.push_back(face(0, 10, 11));

	// 2
	faces.push_back(face(1, 5, 9));
	faces.push_back(face(5, 11, 4));
	faces.push_back(face(11, 10, 2));
	faces.push_back(face(10, 7, 6));
	faces.push_back(face(7, 1, 8));

	// 3
	faces.push_back(face(3, 9, 4));
	faces.push_back(face(3, 4, 2));
	faces.push_back(face(3, 2, 6));
	faces.push_back(face(3, 6, 8));
	faces.push_back(face(3, 8, 9));

	// 4
	faces.push_back(face(4, 9, 5));
	faces.push_back(face(2, 4, 11));
	faces.push_back(face(6, 2, 10));
	faces.push_back(face(8, 6, 7));
	faces.push_back(face(9, 8, 1));

	for (int i = 0; i < recursionLevel; ++i)
	{
		std::vector<face> faces2;

		for (auto v = faces.begin(); v != faces.end(); ++v)
		{
			int a = GetMiddlePoint(v->v1, v->v2);
			int b = GetMiddlePoint(v->v2, v->v3);
			int c = GetMiddlePoint(v->v3, v->v1);

			faces2.push_back(face(v->v1, a, c));
			faces2.push_back(face(v->v2, b, a));
			faces2.push_back(face(v->v3, c, b));
			faces2.push_back(face(a, b, c));
		}
		faces = faces2;
	}

	for (auto f = faces.begin(); f != faces.end(); ++f)
	{
		indexs.push_back(f->v1);
		indexs.push_back(f->v2);
		indexs.push_back(f->v3);

		/*v_d.push_back(vertexs[f->v1].pos.x);
		v_d.push_back(vertexs[f->v1].pos.y);
		v_d.push_back(vertexs[f->v1].pos.z);
		v_d.push_back(vertexs[f->v1].normal.x);
		v_d.push_back(vertexs[f->v1].normal.y);
		v_d.push_back(vertexs[f->v1].normal.z);
		v_d.push_back(vertexs[f->v1].uv.x);
		v_d.push_back(vertexs[f->v1].uv.y);

		v_d.push_back(vertexs[f->v2].pos.x);
		v_d.push_back(vertexs[f->v2].pos.y);
		v_d.push_back(vertexs[f->v2].pos.z);
		v_d.push_back(vertexs[f->v2].normal.x);
		v_d.push_back(vertexs[f->v2].normal.y);
		v_d.push_back(vertexs[f->v2].normal.z);
		v_d.push_back(vertexs[f->v2].uv.x);
		v_d.push_back(vertexs[f->v2].uv.y);

		v_d.push_back(vertexs[f->v3].pos.x);
		v_d.push_back(vertexs[f->v3].pos.y);
		v_d.push_back(vertexs[f->v3].pos.z);
		v_d.push_back(vertexs[f->v3].normal.x);
		v_d.push_back(vertexs[f->v3].normal.y);
		v_d.push_back(vertexs[f->v3].normal.z);
		v_d.push_back(vertexs[f->v3].uv.x);
		v_d.push_back(vertexs[f->v3].uv.y);*/

	}

	for (auto v = vertexs.begin(); v != vertexs.end(); ++v)
	{
		//std::cout << v->pos.x << " " << v->pos.y << " " << v->pos.z << std::endl;
		v_d.push_back(v->pos.x);
		v_d.push_back(v->pos.y);
		v_d.push_back(v->pos.z);
		v_d.push_back(v->normal.x);
		v_d.push_back(v->normal.y);
		v_d.push_back(v->normal.z);
		v_d.push_back(v->uv.x);
		v_d.push_back(v->uv.y);
	}

}

int SphereCreator::AddVertex(glm::vec3 pos)
{
	Vertex v;
	pos = glm::normalize(pos);

	v.pos = pos;
	v.normal = pos;
	v.uv = glm::vec2(0.5f + pos.x / 2.0, 0.5f + pos.y / 2.0);

	vertexs.push_back(v);
	return index++;
}

int SphereCreator::GetMiddlePoint(int p1, int p2)
{
	bool firstIsSmaller = p1 < p2;
	_int64 smallerIndex = firstIsSmaller ? p1 : p2;
	_int64 greaterIndex = firstIsSmaller ? p2 : p1;
	_int64 key = (smallerIndex << 32) + greaterIndex;

	auto re = m_cache.find(key);
	if (re != m_cache.end())
	{
		return re->second;
	}

	Vertex point1 = vertexs[p1];
	Vertex point2 = vertexs[p2];

	glm::vec3 mid = (point1.pos + point2.pos) / 2.0f;
	//std::cout << point1.pos.x << " " << point2.pos.x << "  " << mid.x << std::endl;
	int i = AddVertex(mid);

	m_cache[key] = i;

	return i;
}

glm::vec2 RandomTool::Hammersley(unsigned int i, unsigned int N)
{
	return glm::vec2(float(i)/float(N), RadicalInverse_VdC(i));
}

float RandomTool::RadicalInverse_VdC(unsigned int bits)
{
	bits = (bits << 16u) | (bits >> 16u);
	bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
	bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
	bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
	bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
	return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}

GLenum BufferTool::GetFormatByInternalFormat(GLenum internalFormat)
{
	if (internalFormat == GL_RGB)
		return GL_RGB;
	if (internalFormat == GL_RGB16F)
		return GL_RGB;
	if (internalFormat == GL_RGBA16F)
		return GL_RGBA;
	if (internalFormat == GL_RGB32F)
		return GL_RGB;
	if (internalFormat == GL_RGBA32F)
		return GL_RGBA;
	
}

void BMPTool::WriteBMP(const char* FileName, RGBColor* ColorBuffer, int ImageWidth, int ImageHeight)
{
	//颜色数据总尺寸：
	const int ColorBufferSize = ImageHeight * ImageWidth * sizeof(RGBColor);

	//文件头
	BITMAPFILEHEADER fileHeader;
	fileHeader.bfType = 0x4D42;	//0x42是'B'；0x4D是'M'
	fileHeader.bfReserved1 = 0;
	fileHeader.bfReserved2 = 0;
	fileHeader.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + ColorBufferSize;
	fileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

	//信息头
	BITMAPINFOHEADER bitmapHeader = { 0 };
	bitmapHeader.biSize = sizeof(BITMAPINFOHEADER);
	bitmapHeader.biHeight = ImageHeight;
	bitmapHeader.biWidth = ImageWidth;
	bitmapHeader.biPlanes = 1;
	bitmapHeader.biBitCount = 24;
	bitmapHeader.biSizeImage = ColorBufferSize;
	bitmapHeader.biCompression = 0; //BI_RGB


	FILE* fp;//文件指针

	//打开文件（没有则创建）
	fopen_s(&fp, FileName, "wb");

	if (fp == nullptr)
	{
		std::cout << "creat <" << FileName << "> SnapShot Failure!" << std::endl;
		return;
	}

	//写入文件头和信息头
	fwrite(&fileHeader, sizeof(BITMAPFILEHEADER), 1, fp);
	fwrite(&bitmapHeader, sizeof(BITMAPINFOHEADER), 1, fp);
	//写入颜色数据
	fwrite(ColorBuffer, ColorBufferSize, 1, fp);

	fclose(fp);
}

void BMPTool::ReadBMP(const char* FileName, RGBColor* ColorBuffer)
{
	FILE* fp;

	int ret = fopen_s(&fp, FileName, "rb");
	if (fp == 0)
	{
		std::cout << "Read File Open Fail" << std::endl;
		return;
	}

	BITMAPFILEHEADER fileheader = { 0 };
	fread(&fileheader, sizeof(fileheader), 1, fp);
	if (fileheader.bfType != 0x4D42)
	{
		fclose(fp);
		return;
	}

	BITMAPINFOHEADER head;
	fread(&head, sizeof(BITMAPINFOHEADER), 1, fp);
	LONG bmpWidth = head.biWidth;
	LONG bmpHeight = head.biHeight;
	WORD biBitCount = head.biBitCount;

	size_t size = 0;
	while (true)
	{
		int iret = fread(&ColorBuffer[size], sizeof(RGBColor), 1, fp);
		if (iret == 0)
			break;
		//std::cout << (int)ColorBuffer[size].R <<' '<< (int)ColorBuffer[size].G << ' ' << (int)ColorBuffer[size].B << std::endl;
		size = size + iret;
	}
	fclose(fp);
}

void BMPTool::GetScreenShot(METHOD_TYPE type_, int scene_id, int frame)
{
	//申请颜色数据内存
	RGBColor* ColorBuffer = new RGBColor[CONFIG::SCREEN_CONFIG::SCR_WIDTH * CONFIG::SCREEN_CONFIG::SCR_HEIGHT];

	//读取像素（注意这里的格式是 BGR）
	glReadPixels(0, 0, CONFIG::SCREEN_CONFIG::SCR_WIDTH, CONFIG::SCREEN_CONFIG::SCR_HEIGHT, GL_BGR, GL_UNSIGNED_BYTE, ColorBuffer);

	std::string fileName = "Output\\Scene(" + std::to_string(scene_id)+")\\";

	if (type_ == METHOD_TYPE::GroudTruth)
		fileName += "GroudTruth\\";
	if (type_ == METHOD_TYPE::MyMethod)
		fileName += "MyMethod\\";
	if (type_ == METHOD_TYPE::ContrastMethod_T)
		fileName += "ContrastMethod_T\\";
	if (type_ == METHOD_TYPE::ContrastMethod_F)
		fileName += "ContrastMethod_F\\";

	fileName = fileName  + std::to_string(frame) + ".bmp";

	//std::cout << fileName << std::endl;
	
	//将数据写入文件
	WriteBMP(fileName.c_str(), ColorBuffer, CONFIG::SCREEN_CONFIG::SCR_WIDTH, CONFIG::SCREEN_CONFIG::SCR_HEIGHT);

	//清理颜色数据内存
	delete[] ColorBuffer;

}

void BMPTool::GetQualityResult(int scene_id, int frame)
{
	//申请颜色数据内存
	RGBColor* ColorBuffer_N = new RGBColor[CONFIG::SCREEN_CONFIG::SCR_WIDTH * CONFIG::SCREEN_CONFIG::SCR_HEIGHT]{'a','b','c'};
	RGBColor* ColorBuffer_T = new RGBColor[CONFIG::SCREEN_CONFIG::SCR_WIDTH * CONFIG::SCREEN_CONFIG::SCR_HEIGHT]{ 'a','b','c' };
	RGBColor* ColorBuffer_S = new RGBColor[CONFIG::SCREEN_CONFIG::SCR_WIDTH * CONFIG::SCREEN_CONFIG::SCR_HEIGHT]{ 'a','b','c' };
	RGBColor* ColorBuffer_ST = new RGBColor[CONFIG::SCREEN_CONFIG::SCR_WIDTH * CONFIG::SCREEN_CONFIG::SCR_HEIGHT]{ 'a','b','c' };

	std::string fileNameBase = "Output\\Scene(" + std::to_string(scene_id) + ")\\";
	std::string fileName_N = fileNameBase + "GroudTruth\\" + std::to_string(frame) + ".bmp";
	std::string fileName_T = fileNameBase + "ContrastMethod_T\\" + std::to_string(frame) + ".bmp";
	std::string fileName_S = fileNameBase + "ContrastMethod_F\\" + std::to_string(frame) + ".bmp";
	std::string fileName_ST = fileNameBase + "MyMethod\\" + std::to_string(frame) + ".bmp";

	ReadBMP(fileName_N.c_str(), ColorBuffer_N);
	ReadBMP(fileName_T.c_str(), ColorBuffer_T);
	ReadBMP(fileName_S.c_str(), ColorBuffer_S);
	ReadBMP(fileName_ST.c_str(), ColorBuffer_ST);

	std::string ResultNameBase = "Output\\QualityResult\\Scene(" + std::to_string(scene_id) + ")\\";
	std::string ResultName_N_T = ResultNameBase + "N_T\\" + std::to_string(frame) + ".bmp";
	std::string ResultName_N_S = ResultNameBase + "N_S\\" + std::to_string(frame) + ".bmp";
	std::string ResultName_N_ST = ResultNameBase + "N_ST\\" + std::to_string(frame) + ".bmp";

	RGBColor* ColorBuffer_N_T = new RGBColor[CONFIG::SCREEN_CONFIG::SCR_WIDTH * CONFIG::SCREEN_CONFIG::SCR_HEIGHT];
	RGBColor* ColorBuffer_N_S = new RGBColor[CONFIG::SCREEN_CONFIG::SCR_WIDTH * CONFIG::SCREEN_CONFIG::SCR_HEIGHT];
	RGBColor* ColorBuffer_N_ST = new RGBColor[CONFIG::SCREEN_CONFIG::SCR_WIDTH * CONFIG::SCREEN_CONFIG::SCR_HEIGHT];

	float e1 = 0.0f, e2 = 0.0f, e3 = 0.0f;
	float e1_m = 0.0f, e2_m = 0.0f, e3_m = 0.0f;

	ComputeQualityError(ColorBuffer_N_T, ColorBuffer_N, ColorBuffer_T, e1, e1_m);
	ComputeQualityError(ColorBuffer_N_S, ColorBuffer_N, ColorBuffer_S, e2, e2_m);
	ComputeQualityError(ColorBuffer_N_ST, ColorBuffer_N, ColorBuffer_ST, e3, e3_m);

	WriteBMP(ResultName_N_S.c_str(), ColorBuffer_N_S, CONFIG::SCREEN_CONFIG::SCR_WIDTH, CONFIG::SCREEN_CONFIG::SCR_HEIGHT);
	WriteBMP(ResultName_N_T.c_str(), ColorBuffer_N_T, CONFIG::SCREEN_CONFIG::SCR_WIDTH, CONFIG::SCREEN_CONFIG::SCR_HEIGHT);
	WriteBMP(ResultName_N_ST.c_str(), ColorBuffer_N_ST, CONFIG::SCREEN_CONFIG::SCR_WIDTH, CONFIG::SCREEN_CONFIG::SCR_HEIGHT);

	FILE* fp_N_T;
	FILE* fp_N_S;
	FILE* fp_N_ST;
	std::string writeMode = "wb";
	if (frame > 0)
	{
		writeMode = "ab";
	}

	fopen_s(&fp_N_T, (ResultNameBase + "N_T\\ErrorResult.txt").c_str(), writeMode.c_str());
	if (fp_N_T == nullptr)
	{
		std::cout << "creat <" << ResultNameBase + "N_T\\ErrorResult.txt" << "> ErrorFile Failure!" << std::endl;
		return;
	}

	fopen_s(&fp_N_S, (ResultNameBase + "N_S\\ErrorResult.txt").c_str(), writeMode.c_str());
	if (fp_N_S == nullptr)
	{
		std::cout << "creat <" << ResultNameBase + "N_S\\ErrorResult.txt" << "> ErrorFile Failure!" << std::endl;
		return;
	}

	fopen_s(&fp_N_ST, (ResultNameBase + "N_ST\\ErrorResult.txt").c_str(), writeMode.c_str());
	if (fp_N_ST == nullptr)
	{
		std::cout << "creat <" << ResultNameBase + "N_ST\\ErrorResult.txt" << "> ErrorFile Failure!" << std::endl;
		return;
	}

	std::cout << e1 << ' ' << e2 << ' ' << e3 << std::endl;

	std::string Line_N_T = ""; Line_N_T = Line_N_T + "Frame(" + std::to_string(frame) + ") : Total Error(" + std::to_string(e1) + "), MaxPixelError(" + std::to_string(e1_m) + ") " + '\n';
	std::string Line_N_S = ""; Line_N_S = Line_N_S + "Frame(" + std::to_string(frame) + ") : Total Error(" + std::to_string(e2) + "), MaxPixelError(" + std::to_string(e2_m) + ") " + '\n';
	std::string Line_N_ST = ""; Line_N_ST = Line_N_ST + "Frame(" + std::to_string(frame) + ") : Total Error(" + std::to_string(e3) + "), MaxPixelError(" + std::to_string(e3_m) + ") " + '\n';
	fwrite(Line_N_T.c_str(), Line_N_T.size(), 1, fp_N_T);
	fwrite(Line_N_S.c_str(), Line_N_S.size(), 1, fp_N_S);
	fwrite(Line_N_ST.c_str(), Line_N_ST.size(), 1, fp_N_ST);
	
	fclose(fp_N_T);
	fclose(fp_N_S);
	fclose(fp_N_ST);

	delete[] ColorBuffer_N;
	delete[] ColorBuffer_T;
	delete[] ColorBuffer_S;
	delete[] ColorBuffer_ST;
	delete[] ColorBuffer_N_T;
	delete[] ColorBuffer_N_S;
	delete[] ColorBuffer_N_ST;
}

// 0 : L1
// 1 : L2 
#define ErrorShowType 0
void BMPTool::ComputeQualityError(RGBColor* ColorBufferA, RGBColor* ColorBufferB, RGBColor* ColorBufferC, float& errorL2, float& errorL2_m)
{
	for (int i = 0; i < CONFIG::SCREEN_CONFIG::SCR_WIDTH * CONFIG::SCREEN_CONFIG::SCR_HEIGHT; i++)
	{
		int R1 = abs((int)(ColorBufferB[i].R) - (int)(ColorBufferC[i].R));
		int G1 = abs((int)(ColorBufferB[i].G) - (int)(ColorBufferC[i].G));
		int B1 = abs((int)(ColorBufferB[i].B) - (int)(ColorBufferC[i].B));
		//std::cout << R1 << ' ' << G1 << ' ' << B1 << std::endl;
		//std::cout << (int)(ColorBufferB[i].R) << ' ' << (int)(ColorBufferB[i].G) << ' ' << (int)(ColorBufferB[i].B) << std::endl;
#if ErrorShowType == 1
		R1 = R1 * R1; R1 = R1 > 255 ? 255 : R1;
		G1 = G1 * G1; G1 = G1 > 255 ? 255 : G1;
		B1 = B1 * B1; B1 = B1 > 255 ? 255 : B1;
#endif
		ColorBufferA[i].R = (char)R1;
		ColorBufferA[i].G = (char)G1;
		ColorBufferA[i].B = (char)B1;

#if ErrorShowType == 0
		float temp = ((float)R1 / 255.0f + (float)R1 / 255.0f + (float)R1 / 255.0f) / 3.0f;
#elif ErrorShowType == 1
		float temp = (pow((float)R1 / 255.0f, 2.0f) + pow((float)R1 / 255.0f, 2.0f) + pow((float)R1 / 255.0f, 2.0f)) / 3.0f;
#endif
		if (temp > 0.999)continue;
		errorL2 += temp;
		errorL2_m = errorL2_m > temp ? errorL2_m : temp;
	}
}
