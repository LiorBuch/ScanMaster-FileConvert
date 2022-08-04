#include <iostream>
#include <IGESControl_Reader.hxx>
#include <BRepMesh_IncrementalMesh.hxx>
#include <StlAPI_Writer.hxx>
#include <StlAPI_Reader.hxx>
#include <TColStd_HSequenceOfTransient.hxx>
#include <TopoDS_Shape.hxx>
#include <IGESControl_Controller.hxx>
#include <IGESControl_Writer.hxx>

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

int main() {
    Standard_CString str = "C:/Users/liorb/Desktop/goodstuff/testf.igs";
    TopoDS_Shape shape = igesToOCCT(str);
    occt_to_iges(shape);
}