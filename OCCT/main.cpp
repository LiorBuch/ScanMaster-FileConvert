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
#include <RWStl.hxx>
#include <BRepBuilderAPI_Sewing.hxx>
#include <BRepBuilderAPI_MakeShape.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>
#include <BRep_Builder.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakePolygon.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepBuilderAPI_Sewing.hxx>

using namespace std;


TopoDS_Shape iges_to_occt(Standard_CString path_to_iges) {
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

void occt_to_iges(const TopoDS_Shape& sh,string file_path) {
    IGESControl_Controller::Init();
    IGESControl_Writer ICW("MM", 0);
    //creates a writer object for writing in Face mode with  millimeters
    ICW.AddShape(sh);
    //adds shape sh to IGES model
    ICW.ComputeModel();
    Standard_Boolean OK = ICW.Write(file_path.c_str());
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

void occt_to_step(const TopoDS_Shape& shape,string file_path) {
    STEPControl_Writer writer; // init the writer
    writer.Transfer(shape, STEPControl_ManifoldSolidBrep); //transfer the data from the occt to the writer
    writer.Write(file_path.c_str()); // write the stp file
}

Standard_Boolean stl_to_occt(const Standard_CString path_to_stl,TopoDS_Shape& result) {
    Handle(Poly_Triangulation) aMesh = RWStl::ReadFile(path_to_stl);
    if (aMesh.IsNull())
    {
        return Standard_False;
    }

    TopoDS_Vertex aTriVertexes[3];
    TopoDS_Face aFace;
    TopoDS_Wire aWire;
    BRepBuilderAPI_Sewing aSewingTool;
    aSewingTool.Init(1.0e-06, Standard_True);

    TopoDS_Compound aComp;
    BRep_Builder BuildTool;
    BuildTool.MakeCompound(aComp);

    for (Standard_Integer aTriIdx = 1; aTriIdx <= aMesh->NbTriangles(); ++aTriIdx)
    {
        const Poly_Triangle aTriangle = aMesh->Triangle(aTriIdx);

        Standard_Integer anId[3];
        aTriangle.Get(anId[0], anId[1], anId[2]);

        const gp_Pnt aPnt1 = aMesh->Node(anId[0]);
        const gp_Pnt aPnt2 = aMesh->Node(anId[1]);
        const gp_Pnt aPnt3 = aMesh->Node(anId[2]);
        if (!(aPnt1.IsEqual(aPnt2, 0.0))
            && !(aPnt1.IsEqual(aPnt3, 0.0)))
        {
            aTriVertexes[0] = BRepBuilderAPI_MakeVertex(aPnt1);
            aTriVertexes[1] = BRepBuilderAPI_MakeVertex(aPnt2);
            aTriVertexes[2] = BRepBuilderAPI_MakeVertex(aPnt3);

            aWire = BRepBuilderAPI_MakePolygon(aTriVertexes[0], aTriVertexes[1], aTriVertexes[2], Standard_True);
            if (!aWire.IsNull())
            {
                aFace = BRepBuilderAPI_MakeFace(aWire);
                if (!aFace.IsNull())
                {
                    BuildTool.Add(aComp, aFace);
                }
            }
        }
    }

    aSewingTool.Load(aComp);
    aSewingTool.Perform();
    result = aSewingTool.SewedShape();
    if (result.IsNull())
    {
        result = aComp;
    }
    return Standard_True;
}

void occt_to_stl(const TopoDS_Shape& shape) {
   StlAPI api; // init the writer
   bool workwrite = api.Write(shape, "bananaz.stl"); // writes the output
    cout << workwrite;
}

bool auto_convert(string path_to_input_file ,string output_file_path) {
    char dot = '.';
    TopoDS_Shape output_shape;

    //find and cut the output file format
    int dot_index = output_file_path.find(dot);
    string out_format = output_file_path.substr(dot_index+1, output_file_path.size());
    std::transform(out_format.begin(), out_format.end(), out_format.begin(),
        [](unsigned char c) { return std::tolower(c); }); // this will lower case
    //convert from stl to occt shape
    stl_to_occt(path_to_input_file.c_str(), output_shape);
    // decide according to output format what func to use
    if (out_format=="step"||out_format=="stp")
    {
        occt_to_step(output_shape,output_file_path);
        return 1;
    }
    if (out_format=="igs"||out_format=="iges")
    {
        occt_to_iges(output_shape,output_file_path);
        return 1;
    }
    return 0;
}

// test the files
int main() {
    Standard_CString str = "C:/Users/liorb/Desktop/goodstuff/testf.igs";
    mainConvert(str, "C:/Users/liorb/Desktop/goodstuff/banana.iges");
}