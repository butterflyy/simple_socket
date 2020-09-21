echo "%cd%"
cd ..
git clone -b develop https://github.com/butterflyy/externals.git  --single-branch --depth=1

echo "Build glog"
cd externals
git clone -b v0.4.0 https://github.com/google/glog.git --single-branch --depth=1
cd glog
cmake 
msbuild google-glog.sln
cd ..
rename glog glog-0.3.4

echo "Build gtest"
git clone -b release-1.10.0 https://github.com/google/googletest.git  --single-branch --depth=1
cd googletest
cmake CMakeLists.txt
msbuild googletest-distribution.sln
cd ..
rename googletest googletest-release-1.10.0