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
 6.07409;0.09037;-6.61728;,
 -6.07409;0.09037;-6.61728;,
 -6.07409;41.79175;-6.61728;,
 6.07409;41.79175;-6.61728;,
 6.07409;41.79175;6.61728;,
 -6.07409;41.79175;6.61728;,
 -6.07409;0.09037;6.61728;,
 6.07409;0.09037;6.61728;,
 -6.07409;0.09037;-6.61728;,
 -6.07409;0.09037;6.61728;,
 -6.07409;41.79175;6.61728;,
 -6.07409;41.79175;-6.61728;,
 6.07409;0.09037;6.61728;,
 6.07409;0.09037;-6.61728;,
 6.07409;41.79175;-6.61728;,
 6.07409;41.79175;6.61728;,
 9.19446;41.97037;-9.19446;,
 -9.19446;41.97037;-9.19446;,
 -9.19446;63.88291;-9.19446;,
 9.19446;63.88291;-9.19446;,
 -9.19446;63.88291;9.19446;,
 9.19446;63.88291;9.19446;,
 9.19446;63.88291;9.19446;,
 -9.19446;63.88291;9.19446;,
 -9.19446;41.97037;9.19446;,
 9.19446;41.97037;9.19446;,
 9.19446;41.97037;9.19446;,
 -9.19446;41.97037;9.19446;,
 -9.19446;41.97037;-9.19446;,
 9.19446;41.97037;-9.19446;,
 -9.19446;41.97037;9.19446;,
 -9.19446;63.88291;9.19446;,
 9.19446;41.97037;9.19446;,
 9.19446;63.88291;9.19446;;
 
 10;
 4;0,1,2,3;,
 4;4,5,6,7;,
 4;8,9,10,11;,
 4;12,13,14,15;,
 4;16,17,18,19;,
 4;19,18,20,21;,
 4;22,23,24,25;,
 4;26,27,28,29;,
 4;17,30,31,18;,
 4;32,16,19,33;;
 
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
    "data\\TEXTURE\\crowd07.jpg";
   }
  }
 }
 MeshNormals {
  6;
  0.000000;0.000000;-1.000000;,
  0.000000;0.000000;1.000000;,
  0.000000;1.000000;0.000000;,
  -1.000000;0.000000;0.000000;,
  1.000000;0.000000;0.000000;,
  0.000000;-1.000000;0.000000;;
  10;
  4;0,0,0,0;,
  4;1,1,1,1;,
  4;3,3,3,3;,
  4;4,4,4,4;,
  4;0,0,0,0;,
  4;2,2,2,2;,
  4;1,1,1,1;,
  4;5,5,5,5;,
  4;3,3,3,3;,
  4;4,4,4,4;;
 }
 MeshTextureCoords {
  34;
  0.093860;0.954374;,
  0.314054;0.954374;,
  0.314054;0.420236;,
  0.093860;0.420236;,
  0.500647;0.042038;,
  0.377477;0.040752;,
  0.374714;0.305476;,
  0.497883;0.306762;,
  0.525718;0.301991;,
  0.641038;0.301991;,
  0.641038;0.039641;,
  0.525718;0.039641;,
  0.029574;0.312132;,
  0.139495;0.312132;,
  0.139495;0.033936;,
  0.029574;0.033936;,
  0.641858;0.973465;,
  0.804316;0.973465;,
  0.804316;0.811007;,
  0.641858;0.811007;,
  0.804316;0.648549;,
  0.641858;0.648549;,
  0.804330;0.486105;,
  0.641872;0.486077;,
  0.641844;0.648535;,
  0.804302;0.648563;,
  0.641858;0.486091;,
  0.804316;0.486091;,
  0.804316;0.323633;,
  0.641858;0.323633;,
  0.966774;0.973465;,
  0.966774;0.811007;,
  0.479400;0.973465;,
  0.479400;0.811007;;
 }
}
