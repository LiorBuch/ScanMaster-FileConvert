#include <iostream>
#include <IGESControl_Reader.hxx>
#include <BRepMesh_IncrementalMesh.hxx>
#include <StlAPI_Writer.hxx>
#include <StlAPI_Reader.hxx>
#include <TColStd_HSequenceOfTransient.hxx>
#include <TopoDS_Shape.hxx>
#include <IGESControl_Controller.hxx>
#include <IGESControl_Writer.hxx>
#include <STEPControl_Reader.hxx> 
#include <TopoDS_Shape.hxx> 
#include <BRepTools.hxx> 
#include <string.h>
#include <STEPControl_Writer.hxx>
#include <STEPControl_Controller.hxx>
#include <StlAPI.hxx>
#include <StlAPI_Reader.hxx>
#include <StlAPI_Writer.hxx>

using namespace std;


TopoDS_Shape igesToOCCT(Standard_CString path_to_iges) {
    IGESControl_Reader myIgesReader;
    Standard_Integer nIgesFaces, nTransFaces;

    myIgesReader.ReadFile(path_to_iges);
    //loads file MyFile.igs

    Handle(TColStd_HSequenceOfTransient) myList = myIgesReader.GiveList("iges-faces");
    //selects all IGES faces in the file and puts them into a list  called //MyList,

    nIgesFaces = myList->Length();
    nTransFaces = myIgesReader.TransferList(myList);
    //translates MyList,

    std::cout << "IGES Faces: " << nIgesFaces << "   Transferred:" << nTransFaces << std::endl;
    TopoDS_Shape sh = myIgesReader.OneShape();
    //and obtains the results in an OCCT shape.
    return sh;
}

void occt_to_iges(const TopoDS_Shape& sh) {
    IGESControl_Controller::Init();
    IGESControl_Writer ICW("MM", 0);
    //creates a writer object for writing in Face mode with  millimeters
    ICW.AddShape(sh);
    //adds shape sh to IGES model
    ICW.ComputeModel();
    Standard_Boolean OK = ICW.Write("MyFile.igs");
    //writes a model to the file MyFile.igs
}


TopoDS_Shape step_to_occt(Standard_CString path_to_step)
{
    STEPControl_Reader reader;
    reader.ReadFile(path_to_step);

    // Loads file MyFile.stp 
    Standard_Integer NbRoots = reader.NbRootsForTransfer();

    // gets the number of transferable roots 
    std::cout<<"Number of roots in STEP file : " <<NbRoots <<std::endl;

    Standard_Integer NbTrans = reader.TransferRoots();
    // translates all transferable roots, and returns the number of    //successful translations 
    cout<<"STEP roots transferred : "<< NbTrans<<std::endl;
    cout << "Number of resulting shapes is : "<<reader.NbShapes() <<endl;

    TopoDS_Shape result = reader.OneShape();
    // obtain the results of translation in one OCCT shape 
    return result;
}

void occt_to_step(const TopoDS_Shape& shape) {
    STEPControl_Writer writer; // init the writer
    writer.Transfer(shape, STEPControl_ManifoldSolidBrep); //transfer the data from the occt to the writer
    writer.Write("remiout.step"); // write the stp file
}

TopoDS_Shape stl_to_occt(const Standard_CString path_to_stl) {
    StlAPI api;// init the stl reader
    TopoDS_Shape result; // init the occt obj
    bool workread = api.Read(result, path_to_stl); // reads the stl into the occt obj
    cout << workread << endl;
    return result;
}

void occt_to_stl(const TopoDS_Shape& shape) {
    StlAPI api; // init the writer
    bool workwrite = api.Write(shape, "C:/Users/liorb/source/repos/OCCT/OCCT/bananaz.stl"); // writes the output
    cout << workwrite;
}

BOOLEAN mainConvert(string pathToInputFile ,string outputFileFormat) {
//Todo implament stl to step
    // look for extansion
    // 

    return 1;
}

// test the files
int main() {
    Standard_CString str = "C:/Users/liorb/Desktop/goodstuff/banana.stl";
    TopoDS_Shape shape = stl_to_occt(str);
    occt_to_stl(shape);
}