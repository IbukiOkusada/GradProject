xof 0302txt 0064
template Header {
 <3D82AB43-62DA-11cf-AB39-0020AF71E433>
 WORD major;
 WORD minor;
 DWORD flags;
}

template Vector {
 <3D82AB5E-62DA-11cf-AB39-0020AF71E433>
 FLOAT x;
 FLOAT y;
 FLOAT z;
}

template Coords2d {
 <F6F23F44-7686-11cf-8F52-0040333594A3>
 FLOAT u;
 FLOAT v;
}

template Matrix4x4 {
 <F6F23F45-7686-11cf-8F52-0040333594A3>
 array FLOAT matrix[16];
}

template ColorRGBA {
 <35FF44E0-6C7C-11cf-8F52-0040333594A3>
 FLOAT red;
 FLOAT green;
 FLOAT blue;
 FLOAT alpha;
}

template ColorRGB {
 <D3E16E81-7835-11cf-8F52-0040333594A3>
 FLOAT red;
 FLOAT green;
 FLOAT blue;
}

template IndexedColor {
 <1630B820-7842-11cf-8F52-0040333594A3>
 DWORD index;
 ColorRGBA indexColor;
}

template Boolean {
 <4885AE61-78E8-11cf-8F52-0040333594A3>
 WORD truefalse;
}

template Boolean2d {
 <4885AE63-78E8-11cf-8F52-0040333594A3>
 Boolean u;
 Boolean v;
}

template MaterialWrap {
 <4885AE60-78E8-11cf-8F52-0040333594A3>
 Boolean u;
 Boolean v;
}

template TextureFilename {
 <A42790E1-7810-11cf-8F52-0040333594A3>
 STRING filename;
}

template Material {
 <3D82AB4D-62DA-11cf-AB39-0020AF71E433>
 ColorRGBA faceColor;
 FLOAT power;
 ColorRGB specularColor;
 ColorRGB emissiveColor;
 [...]
}

template MeshFace {
 <3D82AB5F-62DA-11cf-AB39-0020AF71E433>
 DWORD nFaceVertexIndices;
 array DWORD faceVertexIndices[nFaceVertexIndices];
}

template MeshFaceWraps {
 <4885AE62-78E8-11cf-8F52-0040333594A3>
 DWORD nFaceWrapValues;
 Boolean2d faceWrapValues;
}

template MeshTextureCoords {
 <F6F23F40-7686-11cf-8F52-0040333594A3>
 DWORD nTextureCoords;
 array Coords2d textureCoords[nTextureCoords];
}

template MeshMaterialList {
 <F6F23F42-7686-11cf-8F52-0040333594A3>
 DWORD nMaterials;
 DWORD nFaceIndexes;
 array DWORD faceIndexes[nFaceIndexes];
 [Material]
}

template MeshNormals {
 <F6F23F43-7686-11cf-8F52-0040333594A3>
 DWORD nNormals;
 array Vector normals[nNormals];
 DWORD nFaceNormals;
 array MeshFace faceNormals[nFaceNormals];
}

template MeshVertexColors {
 <1630B821-7842-11cf-8F52-0040333594A3>
 DWORD nVertexColors;
 array IndexedColor vertexColors[nVertexColors];
}

template Mesh {
 <3D82AB44-62DA-11cf-AB39-0020AF71E433>
 DWORD nVertices;
 array Vector vertices[nVertices];
 DWORD nFaces;
 array MeshFace faces[nFaces];
 [...]
}

Header{
1;
0;
1;
}

Mesh {
 34;
 -10.86034;37.15816;-11.26999;,
 -10.80486;37.25267;11.07003;,
 -10.54751;59.12227;10.97285;,
 -10.60299;59.02776;-11.36717;,
 11.79123;58.87009;10.91849;,
 11.73576;58.77558;-11.42154;,
 11.73576;58.77558;-11.42154;,
 11.79123;58.87009;10.91849;,
 11.53389;37.00049;11.01566;,
 11.47841;36.90598;-11.32436;,
 11.47841;36.90598;-11.32436;,
 11.53389;37.00049;11.01566;,
 -10.80486;37.25267;11.07003;,
 -10.86034;37.15816;-11.26999;,
 11.53389;37.00049;11.01566;,
 11.79123;58.87009;10.91849;,
 11.47841;36.90598;-11.32436;,
 11.73576;58.77558;-11.42154;,
 -5.97038;0.10668;-6.21551;,
 -5.93944;0.17365;6.24410;,
 -5.59861;36.97577;6.11540;,
 -5.62954;36.90879;-6.34421;,
 6.28129;36.73795;-6.37318;,
 6.31222;36.80492;6.08643;,
 5.97139;0.00281;6.21512;,
 5.94046;-0.06417;-6.24448;,
 -5.93944;0.17365;6.24410;,
 5.97139;0.00281;6.21512;,
 6.31222;36.80492;6.08643;,
 -5.59861;36.97577;6.11540;,
 5.94046;-0.06417;-6.24448;,
 -5.97038;0.10668;-6.21551;,
 -5.62954;36.90879;-6.34421;,
 6.28129;36.73795;-6.37318;;
 
 10;
 4;0,1,2,3;,
 4;3,2,4,5;,
 4;6,7,8,9;,
 4;10,11,12,13;,
 4;1,14,15,2;,
 4;16,0,3,17;,
 4;18,19,20,21;,
 4;22,23,24,25;,
 4;26,27,28,29;,
 4;30,31,32,33;;
 
 MeshMaterialList {
  1;
  10;
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0;;
  Material {
   0.800000;0.800000;0.800000;1.000000;;
   5.000000;
   0.000000;0.000000;0.000000;;
   0.000000;0.000000;0.000000;;
   TextureFilename {
    "data\\TEXTURE\\crowd02.jpg";
   }
  }
 }
 MeshNormals {
  10;
  -0.999928;0.011777;0.002433;,
  0.011278;0.999927;-0.004258;,
  0.999928;-0.011777;-0.002433;,
  -0.999954;0.009269;0.002433;,
  0.999954;-0.009269;-0.002432;,
  -0.011278;-0.999927;0.004258;,
  0.002483;0.004414;0.999987;,
  -0.002484;-0.004414;-0.999987;,
  0.002482;0.003474;0.999991;,
  -0.002482;-0.003474;-0.999991;;
  10;
  4;0,0,0,0;,
  4;1,1,1,1;,
  4;2,2,2,2;,
  4;5,5,5,5;,
  4;6,6,6,6;,
  4;7,7,7,7;,
  4;3,3,3,3;,
  4;4,4,4,4;,
  4;8,8,8,8;,
  4;9,9,9,9;;
 }
 MeshTextureCoords {
  34;
  0.720550;0.978600;,
  0.849330;0.978600;,
  0.849330;0.849820;,
  0.720550;0.849820;,
  0.849330;0.721030;,
  0.720550;0.721030;,
  0.720220;0.592570;,
  0.849000;0.591910;,
  0.849660;0.720700;,
  0.720880;0.721360;,
  0.720550;0.592240;,
  0.849330;0.592240;,
  0.849330;0.463450;,
  0.720550;0.463450;,
  0.978120;0.978600;,
  0.978120;0.849820;,
  0.591760;0.978600;,
  0.591760;0.849820;,
  0.145840;0.456050;,
  0.302680;0.456050;,
  0.302680;0.015950;,
  0.145840;0.015950;,
  0.694730;0.014380;,
  0.537890;0.015910;,
  0.542090;0.445940;,
  0.698930;0.444410;,
  0.339990;0.442530;,
  0.496840;0.442530;,
  0.496840;0.016710;,
  0.339990;0.016710;,
  0.341600;0.976770;,
  0.498450;0.976770;,
  0.498450;0.513630;,
  0.341600;0.513630;;
 }
}
