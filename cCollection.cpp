/******************************************************************************
 Copyright 2012 Victor Yurkovsky

    This file is part of FPGAsm

    FPGAsm is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    FPGAsm is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with FPGAsm.  If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/

#include "global.h"
#include "cSub.h"
#include "cProto.h"


extern unsigned gLineNo;
#define CLASS cCollection
/*
CLASS::CLASS(int max){
  //  chunk = g_string_chunk_new(32);
    name = (char**)malloc(max*sizeof(char*));
    data = (cDatum**)malloc(max*sizeof(cDatum*));
    size=0;
    alloted=max;
    setdebugname((char*)"unk",4);
#if DEBUG & DEBUG_ALLOC
    fprintf(stderr,"%d:cCollection: allocated %s %d\n",gLineNo,debugname,alloted);

#endif
}
*/
CLASS::CLASS(int max,char*thename,int namelen){
    name = (char**)malloc(max*sizeof(char*));
    data = (cDatum**)malloc(max*sizeof(cDatum*));
    size=0;
    alloted=max;
    setdebugname(thename,namelen);
#if DEBUG & DEBUG_ALLOC
    fprintf(stderr,"%d:cCollection: allocated %s %d\n",gLineNo,debugname,alloted);
#endif
}
/*
CLASS::CLASS(){
    name = (char**)malloc(MAX*sizeof(char*));
    data = (cDatum**)malloc(MAX*sizeof(cDatum*));
    size=0;
    alloted=MAX;
#ifdef DEBUG
    strcpy(debugname,"unknown");
#if DEBUG & DEBUG_ALLOC
    fprintf(stderr,"%d:cCollection: allocated %s %d\n",gLineNo,debugname,alloted);
#endif
#endif
}
*/
CLASS::CLASS(const CLASS& src){
  //fprintf(stderr,"COPY CONSTRUCTOR %d\n",src.size);
    name=src.name; //reuse name array
    //data array will be copied.  That way data can be replaced
    //without affecting the src.
    //fprintf(stderr,"alloted: %d\n",src.alloted);
    data=(cDatum**)malloc(src.alloted*sizeof(cDatum*));
    memcpy(data,src.data,src.alloted*sizeof(cDatum*));
    // fprintf(stderr,"memcyp %p,%p,%d\n",data,src.data,(int)src.alloted*sizeof(cDatum*));
    size=src.size;
    alloted=src.alloted;
    strcpy(debugname,"copy");
#if DEBUG & DEBUG_ALLOC
    fprintf(stderr,"%d:cCollection: allocated %s %d\n",gLineNo,debugname,alloted);
#endif    
}
CLASS::~CLASS(){
  #if DEBUG & DEBUG_ALLOC
    fprintf(stderr,"cCollection: freeing %s %d\n",debugname,alloted);
  #endif
  free(name); //name array
  int i;
  for(i=0;i<size;i++){
    delete data[i];
  }
  free(data);
  #if DEBUG & DEBUG_ALLOC
    fprintf(stderr,"cCollection: freeing %s %d\n",debugname,alloted);
  #endif
}

void CLASS::resize(int newsize){
#ifdef DEBUG
  fprintf(stderr,"resizing to %d\n",newsize);
#endif
  data = (cDatum**)realloc(data, newsize*sizeof(void*));
  name = (char**)realloc(name,newsize*sizeof(void*));
#ifdef DEBUG
  fprintf(stderr,"resized to %d\n",newsize);
#endif
  alloted = newsize;
  
}
int CLASS::add(const char* str,int len,cDatum* dat){
  if (size>=alloted) {
    resize(size*2);
    }
  
  
#ifdef CRAP
#ifdef DEBUG
  if(size>=alloted){
    fprintf(stderr,"cCollection:%s add RANGE ERROR - overwriting RAM\n",debugname?debugname:"unnamed");
    fprintf(stderr,"debugmax is %d, writing at %d\n",debugmax,size);
    fprintf(stderr,"len %d str %s\n",len,str);
    //this->dump(stderr,"DUMPING");
    throw(1);
}
#endif
#endif
  //  printf("ADDING to %s %s at %d, %p\n",debugname?debugname:"???",str,size,data);
  char*p = (char*)malloc(len+1);
  memcpy(p,str,len);
  p[len]=0;
  name[size]=p;
  data[size]=dat;
  return size++;
}
/******************************************************************************
  clone 
  Occasionally we need an item that is just a reference to an existing
  name:value pair.  Clone does just that.  
******************************************************************************/ 
//TODO: rename to addRef
int CLASS::addClone(const char* str,cDatum* dat){
  if (size>=alloted) {
    resize(size*2);
    }
  //  printf("ADDING clone %s at %d\n",str,size);
  name[size]=(char*)str;
  data[size]=dat;
  return size++;
 
}
void CLASS::solidify(){
  //replace builder with properly sized copy of the array
  name= (char**)realloc(name,size*sizeof(char*));
  data= (cDatum**)realloc(data,size*sizeof(cDatum*));
  alloted = size;
  //#ifdef DEBUG
  //debugmax=size;
#ifdef DEBUG
  fprintf(stderr,"Solidified %s to %d elements\n",debugname,size);
#endif
//fprintf(stderr,"PRE-SOLIDIFIED. \n");
//dump(stderr);
//fprintf(stderr,"SOLIDIFIED. \n");
//dump(stderr);
}

int CLASS::find(const char* str){
  return find(str,strlen(str));
}
int CLASS::find(const char* str,int len){
//fprintf(stderr,"cCollection:find(%s,%d)\n",str,len);
  int j;
  for(j=size-1;j>-1;j--){
    if(0==strncmp(str,name[j],len))
      if(0==name[j][len])
        break;
  }
//if(-1==j)
//  fprintf(stderr,"find %s %d failed\n",str,len);
//if(-1!=j)fprintf(stderr,"found:%d %s\n",j,name[j]);
  return j;
}
char* CLASS::getName(int i){
  if(i>=0)
    return name[i];
  else {
#ifdef DEBUG
    fprintf(stderr,"cCollection %s attempted to getName(%d)\n",debugname,i);
#else
    fprintf(stderr,"cCollection attempted to getName(%d)\n",i);
#endif
    throw(0);
  }
}
cDatum* CLASS::getDatum(int i){
  if(i>=0)
    return data[i];
  else {
#ifdef DEBUG
    fprintf(stderr,"cCollection %s attempted to getData(%d)\n",debugname,i);
#else
   fprintf(stderr,"cCollection attempted to getData(%d)\n",i);
#endif
   throw(0);
  }
}
int CLASS::indent=0;
void CLASS::dump(FILE*f,const char* title){
  if(!size) return;
  indent+=2;
  if(title)
    fprintf(f,"%s",title);
  else
    fprintf(f,"collection with %d items:\n",size);
  int i;
  for(i=0;i<size;i++){
    int j;
    for(j=0;j<indent;j++) fprintf(f," ");
    fprintf(f,"%d. '%s' ",i,name[i]);
    if(data[i])
      data[i]->dump(f);
    else {
      fprintf(f,"DATA IS NULL!!!\n"); 
    }
    fprintf(f,"\n");
  }
  indent-=2;
}

void CLASS::setdebugname(char*name,int len){
  strncpy(debugname,name,len);
  debugname[len]=0;
}

/******************************************************************************
 vlogWireDefs
 create wire defintions from pins.
******************************************************************************/
void CLASS::vlogWireDefs(FILE*f,const char*prefix){
  int i; for(i=0;i<size;i++){
    fputs(" wire ",f);
    U32 buswidth = data[i]->pinBusWidth;
    if(buswidth>1)
      fprintf(f,"[%d:0] ",buswidth-1);
    fprintf(f,"%s_%s;\n",prefix,name[i]);
  }
}
/******************************************************************************
 vlogPinDefs
 create wire defintions from pins.
 input a,
 output b
******************************************************************************/
void CLASS::vlogPinDefs(FILE*f){
  int i; for(i=0;i<size;i++){
    cDatum* pin = data[i];
    if(pin->pinDir)
      fputs("  output ",f);
    else
      fputs("  input ",f);
    if(pin->pinBusWidth>1)
      fprintf(f,"[%d:0] ",pin->pinBusWidth-1);
    fputs(name[i],f);
    if(i!=size-1)
      fputs(",\n",f);
    else
      fputs("\n",f);
  }
  
}
/******************************************************************************
 vlogPins
 just list the pins for an inst,comma-sep
 a,b,c
******************************************************************************/
void CLASS::vlogPins(FILE*f,const char*prefix){
  int i; for(i=0;i<size;i++){
    U32 buswidth = data[i]->pinBusWidth;
    fprintf(f,"%s_%s",prefix,name[i]);
    if(buswidth>1)
      fprintf(f,"[%d:0]",buswidth-1);
    if(i!=size-1)
      fputs(",",f);
  }
}
