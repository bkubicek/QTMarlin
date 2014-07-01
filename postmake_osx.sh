#!/bin/bash

set +x

cp -R $QWT_DIR/qwt.framework QTMarlin.app/Contents/Resources
cp $QEXTSERIAL_DIR/src/build/libqextserialport.* QTMarlin.app/Contents/Resources

install_name_tool -change qwt.framework/Versions/6/qwt @loader_path/../Resources/qwt.framework/Versions/6/qwt QTMarlin.app/Contents/MacOS/QTMarlin
install_name_tool -change libqextserialport.1.dylib @loader_path/../Resources/libqextserialport.1.dylib QTMarlin.app/Contents/MacOS/QTMarlin  