//
// Created by admin on 2024/1/24.
//

#include <iomanip>
#include "fileio.h"


bool FileIO::readXml(std::string filePath) {

    m_mode = Mode::eRaytracing;
    m_pRaytracingXml = std::move(xmlexamples.m_pRaytracingXml);
//    m_mode = Mode::eVoxelLST;
//    m_pVoxelLstXml = std::move(xmlexamples.m_pVoxelLstXml);

    return false;

    TiXmlDocument mydoc(filePath.c_str()); // tinyxml.h
    bool isloadOk = mydoc.LoadFile();
    if (!isloadOk)
    {
        std::cout << "could not load the test file.Error:" << mydoc.ErrorDesc() << std::endl;
        exit(1);
    }
    TiXmlElement *RootElement = mydoc.RootElement(); // root node of xml
    TiXmlNode *modeNode = RootElement->FirstChild("Mode");
    TiXmlElement *rayTracingDepthEle = modeNode->FirstChildElement("rendermode");
    Mode  modeInd = (Mode)std::stoi(rayTracingDepthEle->GetText());

    if(modeInd == Mode::eRaytracing)
    {
        m_mode = Mode::eRaytracing;
        m_pRaytracingXml = std::make_shared<RaytracingXml>();
        m_pRaytracingXml->lightxml = readLight(RootElement->FirstChild("Geometry"));
        m_pRaytracingXml->sensorxml = readSensor(RootElement->FirstChild("Geometry"));
        m_pRaytracingXml->spectralxmls = readSpectral(RootElement->FirstChild("Attribute"));
        m_pRaytracingXml->thermalxmls = readThermal(RootElement->FirstChild("Attribute"));
        m_pRaytracingXml->scenexml = readScene(RootElement->FirstChild("Scene"));
        m_pRaytracingXml->settingxml = readSetting(RootElement->FirstChild("Setting"));


    }else if(modeInd == Mode::eVoxelLST){
        m_mode = Mode::eVoxelLST;
        m_pVoxelLstXml = std::make_shared<VoxelLstXml>();

    }
    //readSetting(settingNode);



    return true;


}

void FileIO::readDefined(std::shared_ptr<DefinedIO> & definedio) {

    std::string optfile = "D:\\work\\field\\field\\defined\\optipar.txt";

    definedio->definedDir = m_pVoxelLstXml->definedDir;
    std::string predifineDir = definedio->definedDir+"/defined/";
    std::string infileName = predifineDir + "optipar.txt";
    int num = 1;
    Utils::readascfileinout(infileName,0,0,definedio->m_fluspectCoeff.wl_,num);
    Utils::readascfileinout(infileName,0,1,definedio->m_fluspectCoeff.nr_,num);
    Utils::readascfileinout(infileName,0,2,definedio->m_fluspectCoeff.kab_,num);
    Utils::readascfileinout(infileName,0,3,definedio->m_fluspectCoeff.kca_,num);
    Utils::readascfileinout(infileName,0,4,definedio->m_fluspectCoeff.ks_,num);
    Utils::readascfileinout(infileName,0,5,definedio->m_fluspectCoeff.kw_,num);
    Utils::readascfileinout(infileName,0,6,definedio->m_fluspectCoeff.kdm_,num);
    Utils::readascfileinout(infileName,0,7,definedio->m_fluspectCoeff.phiI_,num);
    Utils::readascfileinout(infileName,0,8,definedio->m_fluspectCoeff.phiII_,num);
    Utils::readascfileinout(infileName,0,9,definedio->m_fluspectCoeff.kcaV_,num);
    Utils::readascfileinout(infileName,0,10,definedio->m_fluspectCoeff.kcaZ_,num);
    Utils::readascfileinout(infileName,0,11,definedio->m_fluspectCoeff.kcant_,num);
    Utils::readascfileinout(infileName,0,12,definedio->m_fluspectCoeff.kcaV2_,num);
    Utils::readascfileinout(infileName,0,13,definedio->m_fluspectCoeff.phi_,num);
    Utils::readascfileinout(infileName,0,14,definedio->m_fluspectCoeff.gsv1_,num);
    Utils::readascfileinout(infileName,0,15,definedio->m_fluspectCoeff.gsv2_,num);
    Utils::readascfileinout(infileName,0,16,definedio->m_fluspectCoeff.gsv3_,num);
    Utils::readascfileinout(infileName,0,17,definedio->m_fluspectCoeff.nw_,num);
   // return false;
}

//bool FileIO::writeTif(std::string outDir, void *images, glm::vec4 angles, std::vector<float> bands,
//                      glm::vec2 resolution) {
//
//
//    int saveImage(std::string outfilepath, std::vector<float> &c,
//                  int width, int height,int band, std::string proj, double trans[6]);
//
//
//return false;
//}


std::vector<SpectralXml> FileIO::readSpectral(TiXmlNode *node, Mode mode) {

    std::vector<SpectralXml> spectralxmls;


    return spectralxmls;
}


std::vector<ThermalXml> FileIO::readThermal(TiXmlNode *node, Mode mode) {

    std::vector<ThermalXml> thermalxmls;


    return thermalxmls;
}


SensorXml FileIO::readSensor(TiXmlNode *node, Mode mode){
    SensorXml sensorxml;

    return sensorxml;
}
LightXml FileIO::readLight(TiXmlNode *node, Mode mode){
    LightXml lightxml;

    return lightxml;
}

SettingXml FileIO::readSetting(TiXmlNode *node, Mode mode){
    SettingXml settingxml;

    return settingxml;
}

SceneXml FileIO::readScene(TiXmlNode *node, Mode mode){

    SceneXml sceneXml;



    return sceneXml;

}

void FileIO::readMeteo(std::shared_ptr<DefinedIO> &defineio,int & n_node,
                       std::vector<Meteo> &meteos, std::vector<AtomCond> &wavesets) {


    auto & meteofile = m_pVoxelLstXml->meteoxml.meteofile;


    
    std::ifstream infile(meteofile);
    std::vector<std::string> fields;
    std::string deli(" "), line;

    std::getline(infile, line);
    fields = Utils::splitt(line, deli);
    n_node = std::stoi(fields[0].c_str());

    int meteoNum = 0;
    float z = 15;
    float sm = 0.25;
    float ea = 15;
    float Ca = 380;
    float Oa = 209;
    float Tsold = 25;
    float SatWater = 0.45;
    float dTime = 1800;

    for (int i = 0; i < n_node; i++)
    {
        std::getline(infile, line);
        fields = Utils::splitt(line, deli);

        Meteo mi;
        mi.t = std::atof(fields[0].c_str());
        mi.Ta = std::atof(fields[1].c_str());
        mi.ea = std::atof(fields[2].c_str());
        mi.p = std::atof(fields[3].c_str());
        mi.u = std::atof(fields[4].c_str());
        mi.Rin = std::atof(fields[5].c_str());
        mi.Rli = std::atof(fields[6].c_str());
        mi.sm = sm;
        mi.z = z;
        mi.ea = ea;
        mi.Ca = Ca;
        mi.Oa = Oa;
//        mi.Tsold = Tsold;
//        mi.SatWater = SatWater;
//        mi.dTime = dTime;

        meteos.emplace_back(mi);
        //m_meteoParams.emplace_back(mi);
    }

    //meta = fileio->m_pVoxelLstXml->meteoxml.meta;

    wavesets.resize(N1+N2);
    for (int i = 0; i <  2001 ; i++)
    {
        wavesets[i].wavelength = 400 + i;
    }
    for (int i = 0; i < 126; i++)
    {
        wavesets[i+2001].wavelength = 2500 + i * 100;
    }
    for (int i = 0; i < 35; i++)
    {
        wavesets[i+2127].wavelength = 16000 + i * 1000;
    }


    // atomCond;
    float  *esun_, *esky_, *fesky_, *fesun_;
    int num = 1;
    // wave_ = Utils::infile2num(predifineDir+'Esk', 0, 0, num);
    esun_ = Utils::readascfile(m_pVoxelLstXml->meteoxml.rinfile, 0, 0, num);
    esky_ = Utils::readascfile(m_pVoxelLstXml->meteoxml.rlifile, 0, 0, num);
    fesky_ = new float[num];
    fesun_ = new float[num];

    float TsEsky = 0, TlEsky = 0, TlEsun = 0, TsEsun = 0, tstot = 0, tltot = 0, temp1, temp2, step;
    int b1 = N1;
    int b2 = N1+N2;

    // �̲�
    for (int i = 0; i < b1 - 1; i++)
    {
        temp1 = (esky_[i] + esky_[i + 1]) / 2.0;
        step =  wavesets[i+1].wavelength -  wavesets[i].wavelength;
        temp2 = (esun_[i] + esun_[i + 1]) / 2.0;
        TsEsky += temp1 * step;
        TsEsun += temp2 * step;
    }
    tstot = (TsEsky + TsEsun) * 0.001;
    for (int i = 0; i < b1; i++)
    {
        fesky_[i] = esky_[i] / tstot;
        fesun_[i] = esun_[i] / tstot;
    }
    // ����
    for (int j = b1; j < b2 - 1; j++)
    {
        temp1 = (esky_[j] + esky_[j + 1]) / 2.0;
        step =   wavesets[j+1].wavelength -  wavesets[j].wavelength;
        temp2 = (esun_[j] + esun_[j + 1]) / 2.0;
        TlEsky += temp1 * step;
        TlEsun += temp2 * step;
    }
    tltot = (TlEsky + TlEsun) * 0.001;
    for (int i = b1; i < b2; i++)
    {
        fesky_[i] = esky_[i] / tltot;
        fesun_[i] = esun_[i] / tltot;
    }

    for (int i = 0; i < b2; i++)
    {
        wavesets[i].direct = fesun_[i];
        wavesets[i].diffuse = fesky_[i];
    }

    delete[] fesky_;
    delete[] fesun_;
    delete[] esun_;
    delete[] esky_;

}



void FileIO::writeENVIdata(std::string projectDir, float *pData, int width, int height, int band,
                           Angle &angle, int knode) {

    std::ostringstream  oss_x;
    oss_x << std::setw(3)<<std::setfill('0')<<angle.vza;
    std::ostringstream  oss_y;
    oss_y << std::setw(3)<<std::setfill('0')<<angle.vaa;
    std::ostringstream  oss_z;
    oss_z << std::setw(3)<<std::setfill('0')<<angle.sza;
    std::ostringstream  oss_w;
    oss_w << std::setw(3)<<std::setfill('0')<<angle.saa;

    std::string outPath = projectDir + "/results/VZA=" + oss_x.str() + "_VAA=" + oss_y.str() +
                          "_SZA=" + oss_z.str() + "_SAA=" + oss_w.str() + ".tif";




    std::string tifName = projectDir + +"/results/VZA=" + oss_x.str() + "_VAA=" + oss_y.str() +
                          "_SZA=" + oss_z.str() + "_SAA=" + oss_w.str() + ".img";
    std::string hdrName = projectDir + +"/results/VZA=" + oss_x.str() + "_VAA=" + oss_y.str() +
                          "_SZA=" + oss_z.str() + "_SAA=" + oss_w.str() + ".hdr";

    std::ofstream outfilet1(tifName.c_str(), std::ios::binary);
    outfilet1.write(reinterpret_cast<const char*>(pData), sizeof(float) * width * height * band);
    outfilet1.close();

    std::ofstream outfile(hdrName);
    if (outfile.is_open())
    {
        outfile << "ENVI" << std::endl;
        outfile << "description = {" << std::endl;
        outfile << " File Imported into ENVI.} " << std::endl;
        outfile << "samples = " << width << std::endl;
        outfile << "lines   = " << height << std::endl;
        outfile << "bands   =  " << band << std::endl;
        outfile << "header offset = 0" << std::endl;
        outfile << "file type = ENVI Standard" << std::endl;
        outfile << "data type = 4" << std::endl;
        outfile << "interleave = bsp" << std::endl;
        outfile << "sensor type = unknown" << std::endl;
        outfile << "byte order = 0" << std::endl;
        outfile << "wavelength units = Unknown" << std::endl;
        outfile.close();
    }
    outfile.close();


}




