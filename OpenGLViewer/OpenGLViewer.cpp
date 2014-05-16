// OpenGLViewer.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "opencv.h"
#include "glview.h"

char filedir[] = "./data"; //データ元ディレクトリ
char dir[] = "sample"; //ディレクトリ名
char filename[] = "points.csv"; //点群ファイル名

/// ---設定--- ///
float velocity = 0.002; //移動速度
float cubeSize = 0.01; //立方体の一辺の長さ
float angle = 0.0; //立方体の回転角
GLdouble cameraDistance = 3.0; //注視点とカメラとの距離
float axisSize = 0.25; //座標軸の長さ
float axisThick_ = 0.005; //座標軸の太さ
int window_w = 1000; //ウインドウサイズ
int window_h = 1000; //ウインドウサイズ
/// ---ここまで--- ///

/// ---マウス操作説明--- ///
//  左ドラッグ : 回転
//  ホイールドラッグ : 縦，横方向の平行移動
//  ホイール : 奥行き方向の平行移動
//  右ドラッグ : ズームイン/アウト 
//  左クリック＋右クリック : 点の座標値(x,y,z)取得 - 距離測定も可能（キーボード操作参照）
/// ---ここまで--- ///

/// ---キーボード操作説明--- ///
//  y : 最初の位置に戻る
//  o : 座標軸表示/非表示
//  c : カメラの注視点を点群の中心に向ける
//  j : 物体を左に傾ける
//  l : 物体を右に傾ける
//  0 : 立方体のサイズ大きく
//  9 : 立方体のサイズ小さく
//  s : 点の座標値(x,y,z)保存 2回目で距離測定 3回目でリセット
//  q : ウィンドウを閉じる 
/// ---ここまで--- ///


Mat dataPoints,dataPoints2;
int window1;
int rows, rows2;
const int cols = 3;
float axisThick;
float cubeSize_;
float velocityKey = 0.02;

int * FLAG;
FILE * fp;

/// ////////////////////////
///  関数プロトタイプ宣言
Mat csvread(const char* filename, const int rows, const int cols);
int readfileLine(char * filename);


/// ////////////////////////
///       main関数
int main(int argc, char** argv)
{
	axisThick = axisThick_;
	cubeSize_ = cubeSize/4.;

	/// Mat dataPoints
	char fname[255];
	sprintf(fname,"%s/%s/%s",filedir,dir,filename);
	rows = readfileLine(fname);
	dataPoints = csvread(fname,rows,cols); //csvから点群座標取得

	/// ファイル書き込み
	sprintf(fname,"%s/%s/Distance_%s",filedir,dir,filename);
	errno_t error;
	error = fopen_s(&fp, fname, "w");
	if(error != 0){
		cout << "ファイルが開けません " << fname << endl;
		exit(1);
	}
	fprintf(fp,"距離,点1x,点1y,点1z,点2x,点2y,点2z\n");

	/// OpenGL
	FLAG = (int *)malloc(sizeof(int) * rows);
	for(int i=0;i<rows;i++){
		FLAG[i] = 0;
	}

	/// OpenGL
	// GLUT initialize
	initFlag();
	initParam();
	Initialize();
	//window1
	glutInit(&argc, argv);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(window_w, window_h);
	glutInitDisplayMode( GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE );

	window1 = glutCreateWindow("Window1");
	glutDisplayFunc(disp);
	glutMouseFunc(mouse);
	glutMotionFunc(drag);
	glutPassiveMotionFunc(passive);
	glutMouseWheelFunc ( MouseWheel ) ;//ホイールコールバック
	glutIdleFunc(myGlutIdle);
	glutKeyboardFunc(glut_keyboard);
	glutIdleFunc(animate);
	glClearColor(0.0, 0.0, 0.0, 0.5); //背景色

	//描写開始
	glutMainLoop();
	
	fclose(fp);

	return 0;
}

#pragma region // --- ファイル扱う関数 ---
/// ////////////////////////////////////////////////
/// csvread : csvファイルをcv::Mat形式で出力
/// filename : ファイル名, rows : 行数, cols : 列数
Mat csvread(const char* filename, const int rows, const int cols){
	cv::Mat_<float> m;
	m = cv::Mat::zeros(rows, cols, CV_32F);

	// CSVファイルを開く
	ifstream ifs(filename);
	string str;
	// ファイルが存在しない場合
	if(ifs.fail()) {
		std::cout << "File does not exist" << std::endl;
		exit(0);
	}

	// 列数に合わせて適宜変数を指定
	float x,y,z;
	int line = 0;
	// 1行ずつ読んでいく
	while(getline(ifs, str)) {
		sscanf_s(str.data(), "%f,%f,%f", &x, &y, &z);
		m.at<float>(line,0) = x;
		m.at<float>(line,1) = y;
		m.at<float>(line,2) = z;
		line ++;
		if(line>=rows){
			break;
		}
	}
	return m;
}


/// /////////////////////////////
/// ファイル読み捨てて行数取得
int readfileLine(char * filename)
{
	int total = 0;

	FILE * fp;
	errno_t error;
	error = fopen_s(&fp, filename, "r");
	if(error != 0){
		cout << "ファイルが開けません " << filename << endl;
		exit(1);
	}
	char s[SHRT_MAX];
	while( fgets( s, SHRT_MAX, fp ) != NULL ){
		total++;
	}

	return total;
}

#pragma endregion