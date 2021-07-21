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
#include "cDevice.h"
#include "cPrim.h"
#define CLASS cDevice 

extern unsigned gLineNo;

CLASS::CLASS()
{
  pSites = new cCollection(1000000,(char*)"sites",5);
  pProtos = new cCollection(MAX,(char*)"protos",6);
  //htPlace=g_hash_table_new(g_str_hash,g_str_equal);
  //htPrims =g_hash_table_new(g_str_hash,g_str_equal);
}
CLASS::~CLASS(){
  //g_hash_table_destroy(htPlace);
  //g_hash_table_destroy(htPrims);
  
}

void CLASS::initialize(){
  // initialize device stuff
}
#include <unistd.h> 

/******************************************************************************/
void CLASS::parse_defs( char*buf){
//printf("-- %s\n",buf);
  if(0==strncmp(buf,"(primitive_defs ",16)){
    // primitive_defs COUNT
    int pdcount;
    sscanf(buf,"(primitive_defs %d",&pdcount);
//fprintf(stderr,"Will parse %d primitive_defs\n",pdcount);

    int i;
    for(i=0;i<pdcount;i++){
      // primitive_def NAME PINS ELEMENTS
      readline();//fgets(buf,1024,f);
   //printf("[%s]\n",buf);
      char name[64];
      int pins;
      int elements;
      sscanf(buf," (primitive_def %s %d %d",(char*)&name,&pins,&elements);
    // create a primitive object and map by name.
      //fprintf(stderr,"primitive %s,%d,%d\n",name,pins,elements);
      cPrim* prim = new cPrim(name,strlen(name));
      prim->paramnames->add("loc",3,cDatum::newInt(0));
      prim->paramnames->add("cfg",3,cDatum::newInt(0)); //TODO: null is OK now
      prim->paramnames->add("cfgfile",7,NULL); //TODO: awkward 
      //------------------------------------------------------------------
      // Primitive's pins are parsed.
      //\t\t(pin RESET RESET output)
      int i;
      for(i=0;i<pins;i++){
        readline();//fgets(buf,1024,f);
        char name[64];
        char name1[64];
        char dir[64];
        sscanf(buf," (pin %s %s %s",(char*)&name,(char*)&name1,(char*)&dir);
        if(0==strncmp(dir,"input",5))
          prim->pins->add(name,strlen(name),cDatum::newPin(0,0));
        else if(0==strncmp(dir,"output",6))
          prim->pins->add(name,strlen(name),cDatum::newPin(1,0) );
        else {
printf("ERROR: pin direction\n");
        }
      }
      // Now commit the pins
      prim->pins->solidify();
      //------------------------------------------------------------------
      // Element name pins
    // Elements are interesting only if there is a 'cfg' line...
      for(i=0;i<elements;i++){
        readline();//fgets(buf,1024,f); //(element line
        char elname[64];
        int  elpins;     //not interesting, just to skip lines...
        if(2==sscanf(buf," (element %s %d",elname,&elpins)){
//printf("OK,elname %s %d\n",elname,elpins);
          //skip element's pins
          int j; for(j=0;j<=elpins;j++) //and one extra for cfg test
		   readline();//fgets(buf,1024,f);
          //is the next line (cfg list
          if(0==strncmp(buf,"\t\t\t(cfg ",8)){
            //OK, this is really interesting....We shall store a
            //long string containing all the possible cfgs...
            char* p = strtok(buf+8,")\n");
// fprintf(stderr,"interesting element %s\n",elname);
           //insert an entry (elname,hashtable of possible el cfgs...)
            prim->cfgval->add(elname,strlen(elname),
              cDatum::newGoodCfgs(p,strlen(p))
            );   
            //add to paramname so we can verify name:val pairs
            //prim->paramnames->add(elname,strlen(elname),cDatum::newInt(0));
           
//fprintf(stderr,"added element %s goodcfgs %s\n",elname,p);
          }
          // now skip the conn lines...
          while(true){
            if(0==strncmp(buf,"\t\t)\n",4))
             break;
            readline();//fgets(buf,1024,f);
          }
        } else {
printf("ERROR: expected (element, got [%s].  index %d out of %d\n",buf,i,elements);
        }
      }
      prim->paramnames->solidify();
      prim->cfgval->solidify();
      readline();//fgets(buf,1024,f); //eat the \t\t(
    }

    
    //THIS KILL RAM!  should resize somehow...TODO:protos.solidify();
  }
  //  fprintf(stderr,"done with primitives\n");
}

/******************************************************************************/
void CLASS::addProto(char*name,int len,cProto* proto) {
//fprintf(stderr,"addProto %d %s\n",len,name);
//make sure there are no duplicates
  if(-1==pProtos->find(name,len)){
    pProtos->add(name,len,cDatum::newProto(proto));
  } else {
    fprintf(stderr,"cDevice::addProto Module %s already exists\n",name);
    throw(1);
  }
}
/******************************************************************************/

int CLASS::idxFindProto(char*name,int len){
  return pProtos->find(name,len);
}
cProto* CLASS::findProto(char*name,int len) {
//protos.dump(stderr);
  int i=pProtos->find(name,len);
  if(-1==i) return 0;
  cDatum*p = pProtos->getDatum(i);
  if(p->type!=TYPE_PROTO){
   fprintf(stderr,"findProto: type mismatch\n");
    throw(1);
  }
  return p->valProto;    
}
/******************************************************************************/
//used by hash table iterator...
void CLASS::listProtos() {
  pProtos->dump(stderr,"protos:\n");
}

const char* CLASS::tileFor( char* primsite){
  int i;
  i=pSites->find(primsite);
if(-1==i){
  fprintf(stderr,"cDevice::tileFor(\"%s\") is impossible\n",primsite);
  throw(1);
  
}
  cDatum* dat = pSites->getDatum(i);
  if(TYPE_TILE != dat->type){
    fprintf(stderr,"cDevice::tileFor %s NOT A TILE\n",primsite);
    throw(1);
  }
  return dat->valStr;
    
}

/******************************************************************************/
void CLASS::parse_tile( char*buf){
  //  fprintf(stderr,"Parsing tile %s\n",buf);
  if(0==strncmp(buf,"\t(tile ",7)){
    // Appears to be a valid tile description...
    int x;
    int y;
    char tilename[64];  //placed tilename sitename
    char whatever[64];
    int count;
    sscanf(buf," (tile %d %d %s %s %d ",&x,&y,(char*)&tilename,(char*)&whatever,&count);
//printf("read: %d %d %s %d\n",x,y,whatever,count);
    int i;
    for(i=0;i<count;i++){
      readline();//fgets(buf,1024,f);
      char sitename[64];
      char sitetype[64];
      char siteconn[64];
      int count;
      sscanf(buf," (primitive_site %s %s %s %d",(char*)&sitename,(char*)&sitetype,(char*)&siteconn,&count);
      //printf("primsite: %s %s %s %d\n",sitename,sitetype,siteconn,count);
      // add sitename/tilename mapping to our device
      //g_hash_table_insert(htPlace,g_strdup(sitename),g_strdup(tilename));
      pSites->add(sitename,strlen(sitename),cDatum::newTile(tilename,strlen(tilename)));
     }
  }
}

void CLASS::parse_xdlrc(const char* fname){
  // parse the file...
  f = fopen(fname,"r");
  if(!f){
    errorIn("parse_xdlrc()");
    fprintf(stderr,"#device file '%s' could not be opened\n",fname);
    error(1);
  }
  unsigned oldlineno = gLineNo;
  gLineNo = lineno = 1;
  buf = (char*)malloc(1024);
  int stage=-1;
  while(true){
    if(!readline()) break;
    switch(stage){
      //----------------------------------------------------------
      case -1:  // look for part name/speed
        if(0==strncmp(buf,"(xdl_resource_report ",21)){
          designator=strtok(buf+21," ");
          designator=strdup(strtok(NULL," "));
          stage=0;
        }
        break;
      //----------------------------------------------------------
      case 0:  // parse until (tiles...
        if(0==strncmp(buf,"(tiles ",7)){
          //TODO: parse tile size...
          stage=1;
        }
        break;
      //----------------------------------------------------------
      case 1: //tile...
        if(0==strncmp(buf,")",1)){
          stage=2;
        } else {
          parse_tile(buf);
        }
        break;
      //----------------------------------------------------------
      case 2: //primitive_def..
        if(0==strncmp(buf,"\t)",2)){
          stage=3;
        } else {
          parse_defs(buf);
          stage=4;
        }
      default:
        break;
    }
  }
  fprintf(stderr,"Parsed %d lines from xdlrc file %s\n",lineno,fname);
  gLineNo=oldlineno;
}

char* CLASS::place(const char* key){
  //return (char*)g_hash_table_lookup(htPlace,key);
throw(100);
  
}


bool CLASS::readline(){
  gLineNo = lineno++;

  char*p = fgets(buf,1024,f);
  //  fprintf(stderr,"%d._.%d.\n",gLineNo,(int)strlen(buf));

  return p;
}


/******************************************************************************
                                                                               
******************************************************************************/ 
void CLASS::errorIn(const char* from){
  fprintf(stderr,"-------------------------------------------------------\n");
  fprintf(stderr,"Error in function cDevice::%s\n",from);
   fprintf(stderr,"Error occured in line %d:\n",lineno);
}
void CLASS::error(U32 xerrno){
//   int spaces = ptr-buf;
  fprintf(stderr,"\n%s",buf);        //print line, complete with cr
//   while(spaces--)
//    fputc(' ',stderr);
//  fputc('^',stderr);
  fputs("\n",stderr);
 fprintf(stderr,"----------------------------------------------------------------------\n");
  throw(xerrno);
}
//device name and speed grade for output
const char* CLASS::getDesignator(){
  return designator;
}
