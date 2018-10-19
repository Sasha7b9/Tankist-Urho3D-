//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*! \page page01 Компиляция

    1. Перед компиляцией создайте создайте символьную ссылку на <b>out/distr/TankistData</b> в каталоге <b>out</b> приблизительно так: <b>mklink /j 
    "c:\My\Tankist\out\TankistData" "c:\My\Tankist\out\distr\TankistData"</b>
    2. Установите Visual Studio 2015.\n
    3. Разархивируйте Urho3D-master.zip в каталог c:\\Urho3D-dbg.\n
    4. Перейдие в каталог Urho3D-dbg и создайте файл build.bat:\n
    call cmake_vs2015.bat generated -DURHO3D_64BIT=1 -DURHO3D_LUA=0 -DURHO3D_LUAJIT=0 -DURHO3D_SAMPLES=1 -DURHO3D_D3D11=1 -DURHO3D_LIB_TYPE=SHARED 
    -DURHO3D_DATABASE_ODBC=0 -DURHO3D_PCH=1 -DURHO3D_C++11=1 \n
    и запустите его на выполнение.
*/
