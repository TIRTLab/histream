#include "utils.h"

int Utils::readascfileinout(std::string infileName, int skip, int col, std::vector<float> &data, int &num)
{
    std::string line;
    std::vector <std::string> fields;
    std::string deli(" ");

    std::ifstream infile(infileName.c_str());
    if (infile.is_open())
    {
        num = 0;
        if (skip != 0)
        {
            for (int i = 0; i < skip; i++) std::getline(infile, line);
        }
        while (std::getline(infile, line))
        {
            fields = Utils::splitt(line, deli);
            if (int(fields.size()) >= 1)
            {
                num++;
            }
        }
    }
    else std::cout << "Unable to open the file: " << infileName << std::endl;
    infile.close();

//    float* mydata = new float[num];

    data.clear();
    if (num >= 1)
    {
        int jj = 0;
        std::ifstream infilee(infileName.c_str());
//        mydata = new float[num];
        //getline(infilee,line);

        if (skip != 0)
        {
            for (int i = 0; i < skip; i++) std::getline(infilee, line);
        }
        while (std::getline(infilee, line))
        {
            fields = Utils::splitt(line, deli);
            if (int(fields.size()) > col)
            {
//                mydata[jj] = atof(fields[col].c_str());
                data.push_back(atof(fields[col].c_str()));
                jj = jj + 1;
            }
        }
        infilee.close();
    }

    return 1;
}



std::string Utils::getDirectoryPath(const std::string& fullPath)
{
    size_t lastSlash = fullPath.find_last_of('/');
    if(lastSlash!=std::string::npos)
    {
        return fullPath.substr(0,lastSlash);
    }else{
        return "";
    }
}

std::string Utils::getFileName(const std::string& fullPath)
{
    size_t lastSlash = fullPath.find_last_of('/');
    if(lastSlash!=std::string::npos)
    {
        return fullPath.substr(lastSlash+1);
    }else{
        return fullPath;
    }
}

int Utils::readImageinout1(std::string infilename,std::vector<float> &collected,
                           int &width, int &height, int &nband) {


    GDALAllRegister();
    CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");

    GDALDataset *poDataset;
    GDALDriver *poDriver;


    poDataset = (GDALDataset *) GDALOpen(infilename.c_str(), GA_ReadOnly);//
    if (poDataset == NULL) {
        std::cout << "指定的文件不能打开!" << std::endl;
        return 0;
    }
    width = poDataset->GetRasterXSize();          //获取影像信息
    height = poDataset->GetRasterYSize();
    nband = poDataset->GetRasterCount();

    GDALDataType gBand = poDataset->GetRasterBand(1)->GetRasterDataType();
    int nBits = GDALGetDataTypeSize(gBand);


    //std::vector<std::vector<float>> collected(nband);

    GDALRasterBand * poBand = poDataset->GetRasterBand(1);
    float *bandData = (float *)CPLMalloc(sizeof(float)*width*height);
    CPLErr result = poBand->RasterIO(GF_Read,0,0,width,height,bandData,width,height,GDT_Float32,0,0);

    collected.assign(bandData,bandData+width*height);
    CPLFree(bandData);


    // std::cout<<"123"<<std::endl;
//    double geoTransform[6];                       //获取坐标信息
//    poDataset->GetGeoTransform(geoTransform);
//    const char *spatialRef = poDataset->GetProjectionRef();  //获取投影信息

    GDALClose(poDataset);

    return 1;
}

int Utils::readImageinout(std::string infilename,std::vector<std::vector<float>> &collected,
                          int &width, int &height, int &nband) {


    GDALAllRegister();
    CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");

    GDALDataset *poDataset;
    GDALDriver *poDriver;


    poDataset = (GDALDataset *) GDALOpen(infilename.c_str(), GA_ReadOnly);//
    if (poDataset == NULL) {
        std::cout << "指定的文件不能打开!" << std::endl;
        return 0;
    }
    width = poDataset->GetRasterXSize();          //获取影像信息
    height = poDataset->GetRasterYSize();
    nband = poDataset->GetRasterCount();

    GDALDataType gBand = poDataset->GetRasterBand(1)->GetRasterDataType();
    int nBits = GDALGetDataTypeSize(gBand);


    //std::vector<std::vector<float>> collected(nband);
    for(int kband=1;kband < nband+1;kband++)
    {
//        GDALRasterBand * band = poDataset->GetRasterBand(kband);
//        float *bandData = new float(width*height);
//        band->RasterIO(GF_Read,0,0,width,height,bandData,width,height,GDT_Float32,0,0);
//        collected[kband-1].assign(bandData,bandData+width*height);
//        delete [] bandData;

        GDALRasterBand * poBand = poDataset->GetRasterBand(kband);
        float *bandData = (float *)CPLMalloc(sizeof(float)*width*height);
        CPLErr result = poBand->RasterIO(GF_Read,0,0,width,height,bandData,width,height,GDT_Float32,0,0);
        std::vector<float> tempcollected;
        tempcollected.assign(bandData,bandData+width*height);
        collected.push_back(tempcollected);
        CPLFree(bandData);

    }

    double geoTransform[6];                       //获取坐标信息
    poDataset->GetGeoTransform(geoTransform);
    const char *spatialRef = poDataset->GetProjectionRef();  //获取投影信息

    GDALClose(poDataset);

    return 1;
}

std::vector<std::vector<float>> Utils::readImage(std::string infilename) {


    GDALAllRegister();
    CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");

    GDALDataset *poDataset;
    GDALDriver *poDriver;


    poDataset = (GDALDataset *) GDALOpen(infilename.c_str(), GA_ReadOnly);//
    if (poDataset == NULL) {
        std::cout << "指定的文件不能打开!" << std::endl;
        return std::vector<std::vector<float>>();
    }
    int width = poDataset->GetRasterXSize();          //获取影像信息
    int height = poDataset->GetRasterYSize();
    int nBands = poDataset->GetRasterCount();

    GDALDataType gBand = poDataset->GetRasterBand(1)->GetRasterDataType();
    int nBits = GDALGetDataTypeSize(gBand);


    std::vector<std::vector<float>> collected(nBands);
    for(int kband=1;kband < nBands;kband++)
    {
//        GDALRasterBand * band = poDataset->GetRasterBand(kband);
//        float *bandData = new float(width*height);
//        band->RasterIO(GF_Read,0,0,width,height,bandData,width,height,GDT_Float32,0,0);
//        collected[kband-1].assign(bandData,bandData+width*height);
//        delete [] bandData;

        GDALRasterBand * poBand = poDataset->GetRasterBand(kband);
        float *bandData = (float *)CPLMalloc(sizeof(float)*width*height);
        CPLErr result = poBand->RasterIO(GF_Read,0,0,width,height,bandData,width,height,GDT_Float32,0,0);
        collected[kband-1].assign(bandData,bandData+width*height);
        CPLFree(bandData);
    }

    double geoTransform[6];                       //获取坐标信息
    poDataset->GetGeoTransform(geoTransform);
    const char *spatialRef = poDataset->GetProjectionRef();  //获取投影信息

    GDALClose(poDataset);

    return collected;
}


int Utils::saveImage1(std::string outfilepath, std::vector<float> &c,
                     int width, int height,int band, std::string proj, double trans[6]) {

    CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");	// 支持中文路径
    GDALAllRegister();  //注册所有的驱动

//    GDALDataset *poDataset;   //GDAL数据集
//    GDALRasterBand* poBand = poDataset->GetRasterBand(band);
//    GDALDataType type = poBand->GetRasterDataType();

    GDALDriver *driver = GetGDALDriverManager()->GetDriverByName("GTiff");
    GDALDataset *ods = driver->Create(outfilepath.c_str(),width,height,1,GDT_Float32,NULL);
    GDALRasterBand *oBand = ods->GetRasterBand(1);
    //float *bandData = c.data();
    CPLErr result =oBand->RasterIO(GF_Write,0, 0,width,height,c.data(),width,height,GDT_Float32,0,0);

    GDALClose(ods);
    return 1;
}

int Utils::saveImage(std::string outfilepath, std::vector<std::vector<float>> &c,
                      int width, int height,int band, std::string proj, double trans[6]) {

    CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");	// 支持中文路径
    GDALAllRegister();  //注册所有的驱动

//    GDALDataset *poDataset;   //GDAL数据集
//    GDALRasterBand* poBand = poDataset->GetRasterBand(band);
//    GDALDataType type = poBand->GetRasterDataType();

    GDALDriver *driver = GetGDALDriverManager()->GetDriverByName("GTiff");
    GDALDataset *ods = driver->Create(outfilepath.c_str(),width,height,band,GDT_Float32,NULL);
    for(int kband=0;kband<band;kband++) {
        GDALRasterBand *oBand = ods->GetRasterBand(kband+1);
        //float *bandData = c[kband].data();
        CPLErr result = oBand->RasterIO(GF_Write, 0, 0, width, height, c[kband].data(), width, height, GDT_Float32, 0, 0);

    }
    GDALClose(ods);
    return 1;
}

std::vector<std::string> Utils::splitt(std::string& s, std::string& deli)
{
	std::vector <std::string> ret;
	int last = 0;
	int index = s.find_first_of(deli, last);
	int endx = s.find_last_not_of(deli);
	std::string subpart;
	while (index != int(std::string::npos))
	{
		subpart = s.substr(last, index - last);
		if (subpart.size() != 0) ret.push_back(subpart);
		last = index + 1;
		index = s.find_first_of(deli, last);
	}
	if (endx - last > 0) ret.push_back(s.substr(last, endx));
	return ret;
}

float Utils::expint(float x)
{
	int i1 = 1000;
	int i2 = 100000;
	double sum = 0.0, ii;
	for (int i = i1; i < i2; i++)
	{
		ii = i / 1000.0;
		sum = sum + exp(-x * ii) / ii * 0.001;
	}
	return sum;

}

float* Utils::infile2num(std::string infileName, int skip, int col, int& num)
{
	std::string line;
	std::vector <std::string> fields;
	std::string deli(" ");



	std::ifstream infile(infileName.c_str());
	if (infile.is_open())
	{
		num = 0;
		if (skip != 0)
		{
			for (int i = 0; i < skip; i++) std::getline(infile, line);
		}
		while (std::getline(infile, line))
		{
			fields = Utils::splitt(line, deli);
			if (int(fields.size()) >= 1)
			{
				num++;
			}
		}
	}
	else std::cout << "Unable to open the file: " << infileName << std::endl;
	infile.close();

	float* mydata = new float[num];
	if (num >= 1)
	{

		int jj = 0;
		std::ifstream infilee(infileName.c_str());
		mydata = new float[num];
		//getline(infilee,line);

		if (skip != 0)
		{
			for (int i = 0; i < skip; i++) std::getline(infilee, line);
		}
		while (std::getline(infilee, line))
		{
			fields = Utils::splitt(line, deli);
			if (int(fields.size()) > col)
			{
				mydata[jj] = atof(fields[col].c_str());
				jj = jj + 1;
			}
		}
		infilee.close();
	}

	return mydata;
}

int Utils::getMapIndex(std::map<std::string, int> maps, std::string name)
{
	int loc = 0;
	std::map<std::string, int>::iterator it;
	it = maps.find(name);
	if (it != maps.end())
		loc = it->second;
	return loc;
	return 0;
}

bool isnum(char n)
{
	return (n >= '0' && n <= '9');
}
std::vector <int> Utils::findnum(std::string& ch)
{
	std::vector <int> numVec;
	int k = ch.size();
	int* num = new int[k];
	int result;
	int n = 0;
	int i = 0;
	while (n < k) {
		result = 0;
		if (isnum(ch[n]))
		{
			result = ch[n] - '0';
			while (n < k && isnum(ch[++n]))
				result = (ch[n] - '0') + 10 * result;
			num[i++] = result;
		}
		++n;
	}
	for (int j = 0; j < i; ++j)
	{
		numVec.push_back(num[j]);
	}
	return numVec;
}