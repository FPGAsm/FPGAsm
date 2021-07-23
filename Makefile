#/******************************************************************************
# Copyright 2012 Victor Yurkovsky
#
#    This file is part of FPGAsm
#
#    FPGAsm is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    FPGAsm is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with FPGAsm.  If not, see <http://www.gnu.org/licenses/>.
# 
#******************************************************************************/

all:	fpgasm

fpgasm: fpgasm.cpp \
	 global.h global.cpp \
	 cCollection.h cCollection.cpp \
	 cDatum.h cDatum.cpp \
	 cDevice.h cDevice.cpp \
	 cDyn.h cDyn.cpp \
	 cModule.h cModule.cpp \
	 cMultiWire.h cMultiWireBuilder.h cMultiWireWalker.h \
	 cMultiWire.cpp cMultiWireBuilder.cpp cMultiWireWalker.cpp \
	 cParse.h cParse.cpp cParseStream.h cParseStream.cpp  \
	 cProto.h cProto.cpp \
	 cPrim.h cPrim.cpp \
	 cSub.h  cSub.cpp \
	 cQuark.h cQuark.cpp
	g++ -Wall -fpack-struct -o fpgasm \
fpgasm.cpp \
	 global.cpp \
	 cCollection.cpp \
	 cDatum.cpp \
	 cDevice.cpp \
	 cDyn.cpp \
	 cModule.cpp \
	  cMultiWire.cpp cMultiWireBuilder.cpp cMultiWireWalker.cpp \
	 cParse.cpp cParseStream.cpp  \
	 cProto.cpp \
	 cPrim.cpp \
	 cSub.cpp \
	 cQuark.cpp

install:
	cp fpgasm ~/bin












#-fsanitize=address\
