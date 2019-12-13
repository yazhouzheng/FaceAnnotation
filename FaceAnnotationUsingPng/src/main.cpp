#include <iostream>
#include <string>
#include <vector>
#include "SearchMVX.h"
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <dlib/opencv.h>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_processing/render_face_detections.h>
#include <dlib/image_processing.h>
#include <dlib/gui_widgets.h>
#include "tinyxml2.h"

struct Coordination_2D {
    Coordination_2D() :
        x_2D(0), y_2D(0)
    {}

    Coordination_2D(int x, int y) :
        x_2D(x), y_2D(y)
    {}

    int x_2D;
    int y_2D;
};

struct Coordination_3D {
    Coordination_3D() :
        x_3D(0.0), y_3D(0.0), z_3D(0.0)
    {}

    Coordination_3D(double x, double y, double z) :
        x_3D(x), y_3D(y), z_3D(z)
    {}

    double x_3D;
    double y_3D;
    double z_3D;
};

void DetectLandmark(cv::Mat& imgTemp, std::map<int, Coordination_2D>& markerCoordis, std::string mvxPath)
{
    try
    {
        dlib::frontal_face_detector detector = dlib::get_frontal_face_detector();
        dlib::shape_predictor pose_model;
        dlib::deserialize("shape_predictor_68_face_landmarks.dat") >> pose_model;

        dlib::cv_image<dlib::bgr_pixel> cimg(imgTemp);
        std::vector<dlib::rectangle> faces = detector(cimg);
        std::vector<dlib::full_object_detection> shapes; // Find the pose of each face.
        for (unsigned long i = 0; i < faces.size(); ++i)
            shapes.push_back(pose_model(cimg, faces[i]));

        if (!shapes.empty()) {
            for (int i = 0; i < 68; i++) {
                circle(imgTemp, cvPoint(shapes[0].part(i).x(), shapes[0].part(i).y()), 1, cv::Scalar(0, 0, 255), -1);
                Coordination_2D Coordi(shapes[0].part(i).x(), shapes[0].part(i).y());
                markerCoordis[i] = Coordi;
            }
        }
        std::string saveIRName = mvxPath.substr(0, mvxPath.rfind(".")) + "Feature" + ".png";
        cv::imwrite(saveIRName, imgTemp);
    }
    catch (dlib::serialization_error& e)
    {
        std::cout << "You need dlib's default face landmarking model file to run this example." << std::endl
            << "You can get it from the following URL: " << std::endl
            << "http://dlib.net/files/shape_predictor_68_face_landmarks.dat.bz2" << std::endl
            << std::endl << e.what() << std::endl;
    }
    catch (std::exception& e)
    {
        std::cout << e.what() << std::endl;
    }
};

std::string GetSampleID(const std::string& mvxDir)
{
    std::string srcNameWithPostfix = mvxDir.substr(0, mvxDir.rfind("\\"));
    std::string srcName = srcNameWithPostfix.substr(srcNameWithPostfix.rfind("\\") + 1);
    return srcName;
}

void ExportFeatureCoordToXml(const std::map<int, Coordination_2D>& featuresMap, std::string mvxPath)
{
    std::cout << GetSampleID(mvxPath) << std::endl;
    tinyxml2::XMLDocument doc;
    tinyxml2::XMLElement* root = doc.NewElement("sampleID");
    root->SetAttribute("value", GetSampleID(mvxPath).c_str());
    doc.InsertFirstChild(root);
    tinyxml2::XMLElement* features = doc.NewElement("features");

    for (int i = 0; i < 68; i++)
    {
        tinyxml2::XMLElement* feature = doc.NewElement("feature");
        tinyxml2::XMLElement* x = doc.NewElement("x");
        tinyxml2::XMLElement* y = doc.NewElement("y");

        feature->SetAttribute("featureID", i);
        x->SetAttribute("value", featuresMap.at(i).x_2D);
        y->SetAttribute("value", featuresMap.at(i).y_2D);

        feature->InsertFirstChild(x);
        feature->InsertAfterChild(x, y);

        features->InsertEndChild(feature);
    }

    root->InsertFirstChild(features);

    std::string saveXmlName = mvxPath.substr(0, mvxPath.rfind(".")) + "Feature" + ".xml";
    doc.SaveFile(saveXmlName.c_str());

    std::cout << saveXmlName << " saved." << std::endl;
}

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        std::cout << "Usage : FaceAnnotation.exe FolderName.";
        return -1;
    }

    std::vector<std::string> filePaths;
    CSearchMVX searchMvx;
    searchMvx.GetFilePaths(argv[1], filePaths);

    for (std::vector<std::string>::iterator it = filePaths.begin(); it != filePaths.end(); it++)
    {
        std::string depthImg = (*it).substr((*it).rfind("\\") + 1);
        std::cout << "picture name is " << depthImg << std::endl;
        std::cout << "File name is " << *it << std::endl;

        std::map<int, Coordination_2D> faceLandmarkerCoordis_2D;
        cv::Mat personPhoto = cv::imread(*it);
        DetectLandmark(personPhoto, faceLandmarkerCoordis_2D, *it);
        ExportFeatureCoordToXml(faceLandmarkerCoordis_2D, *it);
    }

    return 0;
}