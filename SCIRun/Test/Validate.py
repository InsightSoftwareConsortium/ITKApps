#/!/usr/bin/perl

# This is an example of how to use java and xalan to validate the itk XML files against
# the Insight code. This perl script is meant to be run inside of the SCIRun/Test directory.
# The variables $path_to_SCIRun, $ITK_SRC, and $ITK_BIN should be set specific to your machine.
#       $path_to_SCIRun = the full path to the InsightApplications/SCIRun directory
#       $ITK_SRC = the full path to the Insight source directory (InsightToolkit)
#       $ITK_BIN = the full path to the Inisght binary directory


# Set absolute path to InsightApplications/SCIRun
$path_to_SCIRun = "/home/sci/darbyb/work/projects/SCIRunITK/InsightApplications_XMLVALIDATOR/InsightApplications/SCIRun";

$ITK_SRC = "/home/sci/darbyb/work/projects/SCIRunITK/InsightToolkit-1.4.0";
$ITK_BIN = "/home/sci/darbyb/work/projects/SCIRunITK/InsightBin";

$xmlDir = "$path_to_SCIRun/ITK";

# validate each itk_*.xml file in the ITK directory

opendir(DIR, $xmlDir) or die "can't opendir $xmlDir: $!";

while (defined($oldFile = readdir(DIR))) {
  if (($oldFile ne ".") && ($oldFile ne "..") && ($oldFile ne "itk_filter.dtd")) {
  
  print "VALIDATING: $oldFile\n";
  
  # Remove the validate executable
  $command = "rm validate";
  system($command);
  
  # Run the code generator to get the validate.cc
  $command = "java -classpath $path_to_SCIRun/Thirdparty/xercesImpl.jar:$path_to_SCIRun/Thirdparty/xalan.jar:$path_to_SCIRun/Thirdparty/xml-apis.jar:$path_to_SCIRun/Examples/CodeGenerator SCIRun.GenerateSCIRunCode Validate $xmlDir/$oldFile $path_to_SCIRun/Test/Validate.xsl $path_to_SCIRun/Test/validate.cc";
  system($command);
  
  # Compile it
  $command = "g++ -o $path_to_SCIRun/Test/validate $path_to_SCIRun/Test/validate.cc -L$ITK_BIN/bin -lITKFEM -lITKIO -lITKMetaIO -lITKAlgorithms -lITKStatistics -lITKBasicFilters -lITKCommon  -litkvnl -litkvnl_algo -litknetlib -litksys -lz -I$ITK_BIN -I$ITK_SRC/Code/Algorithms -I$ITK_SRC/Code/BasicFilters -I$ITK_SRC/Code/Common -I$ITK_SRC/Code/Numerics -I$ITK_SRC/Code/IO -I$ITK_SRC/Code/Numerics/FEM -I$ITK_SRC/Code/Numerics/Statistics -I$ITK_SRC/Code/SpatialObject -I$ITK_SRC/Utilities/MetaIO -I$ITK_SRC/Utilities/DICOMParser -I$ITK_BIN/Utilities/expat -I$ITK_SRC/Utilities/expat -I$ITK_BIN/Utilities -I$ITK_SRC/Utilities/vxl/vcl -I$ITK_SRC/Utilities/vxl/v3p/netlib -I$ITK_SRC/Utilities/vxl -I$ITK_BIN/Utilities/vxl/vcl -I$ITK_BIN/Utilities/vxl -I$ITK_SRC/Utilities/zlib -I$ITK_BIN/Utilities/zlib -I$ITK_SRC/Utilities/jpeg -I$ITK_BIN/Utilities/jpeg -I/usr/include";
  system($command);
  
  # Check if validate executable was built
  open($filehandle,"validate") or die "ERROR:\n\t$oldFile is not valid.\n\tPlease update to be in sync with Insight files.\n" ;
  print "\t*** Valid ***\n";
  }
}
closedir(DIR);

