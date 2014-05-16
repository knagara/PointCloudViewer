#include "stdafx.h"
#include "glview.h"

#pragma region // --- OpenGL 変数 ---
const double MY_MINDISTANCE = 0.1;
const double MY_MAXDISTANCE = 100.0;
const double CAMERA_DISTANCE = 30.0; //カメラと座標系の距離

int MouseX,MouseY;
int onMouseX,onMouseY;
bool M_left=false;
bool M_right=false;
bool M_middle=false;
bool M_wheel_up=false;
bool M_wheel_down=false;
bool phiState=true;
int wheel=0;
GLdouble upX = 0.0; //カメラの向きx
GLdouble upY = -1.0; //カメラの向き[1.0か-1.0にする]
GLdouble upZ = 0.0; //カメラの向きz

int saveCount=0, numOfSelected=-999, numOfSelected_s=-999, numOfSelected_s2=-999;
GLdouble wx=0,wy=0,wz=0, wx_s=0,wy_s=0,wz_s=0, wx_s2=0,wy_s2=0,wz_s2=0, wDistance=0;

GLdouble fovy=30.0, onFovy=30.0, fovydif;
GLdouble eyeX, eyeY, eyeZ;
GLdouble eyeR, eyeTheta, eyePhi, onEyeTheta, onEyePhi, theta, phi;
int eyeXleft, eyeXright, eyeYup, eyeYdown, eyeZin, eyeZout;
GLdouble centerX=0, centerY=0, centerZ=0;
GLdouble onCenterX=0, onCenterY=0, onCenterZ=0;
GLdouble centerXdif, centerYdif, centerZdif;
int centerXleft, centerXright, centerYup, centerYdown;
#pragma endregion


#pragma region // --- Material ---
//////////////////////////////////////////
//jade(翡翠)
MaterialStruct ms_jade = {
  {0.135,     0.2225,   0.1575,   1.0},
  {0.54,      0.89,     0.63,     1.0},
  {0.316228,  0.316228, 0.316228, 1.0},
  12.8};

#pragma endregion
  
#pragma region // --- 描写関数 ---
/// /////////////////////////////////////////
///  描写関数　この関数が繰り返し実行される
void disp(){
	
#pragma region // --- マウス操作 ---

	if(M_left && M_right){
	}else if(M_left){
		eyeTheta = (GLdouble)((MouseX - onMouseX)*velocity*2.0);
		eyePhi = (GLdouble)((MouseY - onMouseY)*velocity*2.0);
	}else if(M_middle){
		centerXdif = (GLdouble)((MouseX - onMouseX)*velocity);
		centerYdif = (GLdouble)((MouseY - onMouseY)*velocity);
	}else if(M_right){
		fovydif = (GLdouble)((MouseY - onMouseY)*velocity*20.0);
	}else{
		onCenterX = centerX;
		onCenterY = centerY;
		onCenterZ = centerZ;
		onEyeTheta = theta;
		onEyePhi = phi;
		onFovy = fovy;
		initParamMove();
		if(M_wheel_up==true){
			centerZdif = (GLdouble)velocity * 100;
			M_wheel_up = false;
		}else if(M_wheel_down==true){
			centerZdif = (-1.0) * (GLdouble)velocity * 100;
			M_wheel_down = false;
		}
	}

	fovy = onFovy + fovydif;
	
	theta = onEyeTheta+eyeTheta;
	phi = onEyePhi+eyePhi;

	if(phi>=M_PI*1.5){
		if(phiState==false){
			upY = -upY;
			phiState = true;
			onEyePhi = onEyePhi - M_PI*2.0;
			phi = onEyePhi+eyePhi;
		}
	}else if(phi>=M_PI*0.5){
		if(phiState==true){
			upY = -upY;
			phiState = false;
		}
	}else if(phi<=-M_PI*1.5){
		if(phiState==false){
			upY = -upY;
			phiState = true;
			onEyePhi = onEyePhi + M_PI*2.0;
			phi = onEyePhi+eyePhi;
		}
	}else if(phi<=-M_PI*0.5){
		if(phiState==true){
			upY = -upY;
			phiState = false;
		}
	}else if(phi>-M_PI*0.5 && phi<M_PI*0.5){
		if(phiState==false){
			upY = -upY;
			phiState = true;
		}
	}

	centerX = onCenterX - cos(theta)*centerXdif + sin(phi)*sin(theta)*centerYdif + cos(phi)*sin(theta)*centerZdif;
	centerY = onCenterY                       - cos(phi)*centerYdif           + sin(phi)*centerZdif;
	centerZ = onCenterZ + sin(theta)*centerXdif + sin(phi)*cos(theta)*centerYdif + cos(phi)*cos(theta)*centerZdif;

	eyeX = centerX - eyeR * sin(theta) * cos(phi);
	eyeY = centerY - eyeR * sin(phi);
	eyeZ = centerZ - eyeR * cos(theta) * cos(phi);

#pragma endregion

#pragma region // --- 視点決定 ---
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fovy, 1.0, MY_MINDISTANCE, MY_MAXDISTANCE);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//カメラの座標, 原点, 座標系の向き
	gluLookAt(
		eyeX, eyeY, eyeZ,
		centerX, centerY, centerZ,
		upX, upY, upZ);

	//Initialize();
#pragma endregion

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_COLOR_MATERIAL);


	/// -----------座標軸表示-------------
	glColor3d(1.0, 0.0, 0.0);
	drowCuboid(axisSize,axisThick,axisThick,axisSize/2., 0, 0,ms_jade,45.0,1.0,0.0,0.0);
	glColor3d(0.0, 1.0, 0.0);
	drowCuboid(axisThick,axisSize,axisThick, 0, axisSize/2.,0,ms_jade,45.0,0.0,1.0,0.0);
	glColor3d(0.0, 0.0, 1.0);
	drowCuboid(axisThick,axisThick,axisSize, 0, 0,axisSize/2.,ms_jade,45.0,0.0,0.0,1.0);
	
	/// ---------点群の立方体表示----------
	drawPointsAsCube();
	
	/// ----------文字列---------
	glColor3d(1.0, 1.0, 0.0);
	drawText(0);
	glColor3d(0.0, 0.0, 0.0);
	drawText(2);

	glDisable(GL_LIGHTING);
	glFlush();
	glDisable(GL_DEPTH_TEST);
	glutSwapBuffers();
}
#pragma endregion

void drawText(int offset){
	char str[256];
	//sprintf(str, "(%d,%d)", MouseX, MouseY);
    //DrawString(str, window_w, window_h, 10+offset, window_h - 90+offset);
	sprintf(str, "(%.4f,%.4f,%.4f)", wx,wy,wz);
    DrawString(str, window_w, window_h, 10+offset, window_h - 90+offset);
	sprintf(str, "(%.4f,%.4f,%.4f)", wx_s,wy_s,wz_s);
    DrawString(str, window_w, window_h, 10+offset, window_h - 50+offset);
	sprintf(str, "(%.4f,%.4f,%.4f)", wx_s2,wy_s2,wz_s2);
    DrawString(str, window_w, window_h, 10+offset, window_h - 30+offset);
	sprintf(str, "Distance = %.4f", wDistance);
    DrawString(str, window_w, window_h, 220+offset, window_h - 40+offset);
}

void drawPointsAsCube(){
	
	wx = 0;
	wy = 0;
	wz = 0;
	for(int i=0;i<rows;i++){
		///位置
		GLdouble x = (GLdouble)dataPoints.at<float>(i,0);
		GLdouble y = (GLdouble)dataPoints.at<float>(i,1);
		GLdouble z = (GLdouble)dataPoints.at<float>(i,2);
		///色
		if(FLAG[i]==1){
			glColor3d(1.0, 0.0, 0.0);
			wx = x;
			wy = y;
			wz = z;
		}else if(FLAG[i]==2){
			glColor3d(0.0, 1.0, 0.0);
		}else{
			glColor3d(1.0, 1.0, 1.0);
		}
		///描写
		glPushName(i);
		drowCuboid(cubeSize,cubeSize,cubeSize,x, y, z,ms_jade,angle,1.0,1.0,0.0);
		glPopName();
	}
}


#pragma region // --- Initialize ---
void initFlag(void){
	eyeXleft=0;
	eyeXright=0;
	eyeYup=0;
	eyeYdown=0;
	centerXleft=0;
	centerXright=0;
	centerYup=0;
	centerYdown=0;
	eyeZin=0;
	eyeZout=0;
}

void initParam(void){
	centerX = 0;
	centerY = 0;
	centerZ = 0;
	onCenterX = 0;
	onCenterY = 0;
	onCenterZ = 0;
	centerXdif = 0;
	centerYdif = 0;
	centerZdif = 0;
	onEyeTheta = 0;
	eyeTheta = 0;
	onEyePhi = 0;
	eyePhi = 0;
	theta = 0;
	phi = 0;
	eyeR = cameraDistance;
	fovy = 30.0;
	onFovy = 30.0;
	fovydif = 0;
	upX = 0.0;
	upY = -1.0;
	upZ = 0.0;
	phiState=true;
}

void initParamMove(void){
	centerXdif = 0;
	centerYdif = 0;
	centerZdif = 0;
	eyeTheta = 0;
	eyePhi = 0;
	fovydif = 0;
}

void Initialize(){
	  //光源の設定--------------------------------------
	///GLfloat lightPosition[4] = { 10.0, -10.0, -20.0, 0.0 }; //光源の位置
	///GLfloat lightDirection[3] = { 0.0, 0.0, 0.0}; //光源の方向
	///GLfloat lightDiffuse[3]  = { 1.2,  1.2, 1.2  }; //拡散光
	///GLfloat lightAmbient[3]  = { 0.15, 0.15, 0.15 }; //環境光
	///GLfloat lightSpecular[3] = { 1.0,   1.0, 1.0  }; //鏡面光
	GLfloat lightPosition[4] = { 0.0, -10.0, -5.0, 0.0 }; //光源の位置
	GLfloat lightDirection[3] = { 0.0, 0.0, 0.0}; //光源の方向
	GLfloat lightDiffuse[3]  = { 0.7,  0.7, 0.7  }; //拡散光
	GLfloat lightAmbient[3]  = { 0.25, 0.25, 0.25 }; //環境光
	//GLfloat lightSpecular[3] = { 1.0,   1.0, 1.0  }; //鏡面光

	
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, lightDirection);
	glLightfv(GL_LIGHT0, GL_DIFFUSE,  lightDiffuse);
	glLightfv(GL_LIGHT0, GL_AMBIENT,  lightAmbient);
	//glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);

	//距離減衰
	//glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 0.1);
	//glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.01);
	//glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.01);
}
#pragma endregion


#pragma region // --- マウスによる視点制御 ---
//マウス(クリック)
void mouse(int button, int state, int x, int y)
{

 if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
 {
  M_left=true;
  onMouseX = x;
  onMouseY = y;
 }else if(button == GLUT_LEFT_BUTTON && state == GLUT_UP)
 {
  M_left=false;
 }
 if(button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
 {
  M_right=true;
  onMouseX = x;
  onMouseY = y;
 }else if(button == GLUT_RIGHT_BUTTON && state == GLUT_UP)
 {
  M_right=false;
 }
 if(button == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN)
 {
  M_middle=true;
  onMouseX = x;
  onMouseY = y;
 }else if(button == GLUT_MIDDLE_BUTTON && state == GLUT_UP)
 {
  M_middle=false;
 }

 if(M_left == true && M_right == true){
	  /// マウスピック
	  //float depth = returnDepth(x,y);
	  //CalculateWorldCo(x,y,depth,wx,wy,wz);
	  PICK_UP(x,y);
 }
}

//ドラッグ
void drag(int x, int y)
{
 MouseX=x;
 MouseY=y;
}

//パッシブ
void passive(int x, int y)
{
 MouseX=x;
 MouseY=y;
}
//ホイール
void MouseWheel(int wheel_number, int direction, int x, int y)
{
 if(direction==1){wheel++;M_wheel_up=true;}else{wheel--;M_wheel_down=true;}
}

#pragma endregion

#pragma region // --- マウスピック ---

void PICK_UP(int x, int y)
{

	/*ビューポート取得*/
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);

	/*セレクションバッファ用意*/
#define BUFSIZE 256
	GLuint selectBuf[BUFSIZE];
	glSelectBuffer(BUFSIZE, selectBuf);

	/*レンダーモード変更*/
	glRenderMode(GL_SELECT);

	glInitNames();

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluPickMatrix(x,viewport[3]-y,5.0,5.0, viewport);

	/*視体積*/
	gluPerspective( fovy,(double)viewport[2]/(double)viewport[3] , MY_MINDISTANCE, MY_MAXDISTANCE);

	glMatrixMode( GL_MODELVIEW );
	/// 点群表示
	drawPointsAsCube();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glMatrixMode( GL_MODELVIEW );

	int hits;
	hits = glRenderMode(GL_RENDER);

	SELECT_HITS(hits, selectBuf);

}

int SELECT_HITS(int hits,GLuint *buf)
{
	
	///マウスピック解除
	for(int i=0;i<rows;i++) FLAG[i] = 0;

	/*ヒット数チェック*/
	if(hits<=0)
	{
		//cout << "0 hit ..."<< endl;
		return -1;

	}else
	{
		//cout << hits << " hits !!" << endl;
	}

	int name = buf[3];
	int depth = buf[1];

	for(int loop = 1; loop < hits;loop++)
	{
		if(buf[loop*4+1] < GLuint(depth)) //より手前のものが見つかったとき
		{
			name = buf[loop*4+3];
			depth = buf[loop*4+1];
		}

	}
	
	numOfSelected = name;

	FLAG[name] = 1;
	
	if(numOfSelected_s >= 0){
		FLAG[numOfSelected_s] = 2;
	}
	if(numOfSelected_s2 >= 0){
		FLAG[numOfSelected_s2] = 2;
	}

	//cout << "min name is " << name << endl;

	return 1;
}

/// ////////////////////////////////////////////
/// クリックしたところの描画したもののデプス値を取得する．
float returnDepth(int x, int y){
	float z;
	GLint viewport[4];                      //ビューポート
	//デバイス座標系とウィンドウ座標系の変換
	glGetIntegerv(GL_VIEWPORT,viewport);    //現在のビューポートを代入
	glReadPixels(x,viewport[3]-y,1,1,
	GL_DEPTH_COMPONENT,
	GL_FLOAT,
	&z);
	// デプスバッファの値を返す．
	return z;
}

/// /////////////////////////////////////////////////////////////////
/// クリックした点にあるピクセルのデプス値を用いて，世界座標を割り出す．
void  CalculateWorldCo(int x, int y, float depth,double &wx, double &wy,double &wz){
	// 4*4行列を二つ用意する
	GLdouble mvMatrix[16],pjMatrix[16];
	// ビューポート用配列
	GLint viewport[4];
	// パラメータを取得する．
	glGetIntegerv(GL_VIEWPORT, viewport);
	glGetDoublev(GL_MODELVIEW_MATRIX, mvMatrix);
	glGetDoublev(GL_PROJECTION_MATRIX, pjMatrix);
	//世界座標を取得する
	gluUnProject((double)x,(double)viewport[3]-y,depth,
	mvMatrix,
	pjMatrix,
	viewport,
	&wx,
	&wy,
	&wz);
}
#pragma endregion

#pragma region // --- キーボードによる制御 ---
/// /////////////////////////////////////////
///  キーボードによる制御
void glut_keyboard(unsigned char key, int x, int y){
	//終了
	if(key=='q'){
		exit(0);
	}
	//元に戻す
	if(key=='y'){
		initFlag();
		initParam();
	}
	//座標軸表示切り替え
	if(key=='o'){
		if(axisThick == 0.0){
			axisThick = axisThick_;
		}else{
			axisThick = 0.0;
		}
	}
	//カメラの注視点を点群の中心に向ける
	if(key=='c'){
		Mat mean;
		reduce(dataPoints,mean,0,CV_REDUCE_AVG);
		centerX = mean.at<float>(0,0);
		centerY = mean.at<float>(0,1);
		centerZ = mean.at<float>(0,2);
	}
	//点群のサイズ
	if(key=='0'){
		cubeSize += cubeSize_;
	}else if(key=='9'){
		cubeSize -= cubeSize_;
	}
	//カメラの向き
	if(key=='j'){
		upX += 0.1;
	}else if(key=='l'){
		upX -= 0.1;
	}
	//点群座標保存
	if(key=='s'){
		if(saveCount==0){
			wx_s = wx;
			wy_s = wy;
			wz_s = wz;
			wx_s2 = 0;
			wy_s2 = 0;
			wz_s2 = 0;
			wDistance = 0;
			numOfSelected_s = numOfSelected;
			FLAG[numOfSelected_s] = 2;
		}else if(saveCount==1){
			wx_s2 = wx;
			wy_s2 = wy;
			wz_s2 = wz;
			wDistance = sqrt((wx_s - wx_s2)*(wx_s - wx_s2) +
							(wy_s - wy_s2)*(wy_s - wy_s2) +
							(wz_s - wz_s2)*(wz_s - wz_s2));
			numOfSelected_s2 = numOfSelected;
			FLAG[numOfSelected_s2] = 2;
			fprintf(fp,"%f,%f,%f,%f,%f,%f,%f\n",wDistance,wx_s,wy_s,wz_s,wx_s2,wy_s2,wz_s2);
		}else if(saveCount==2){
			wx_s = 0;
			wy_s = 0;
			wz_s = 0;
			wx_s2 = 0;
			wy_s2 = 0;
			wz_s2 = 0;
			wDistance = 0;
			FLAG[numOfSelected_s] = 0;
			FLAG[numOfSelected_s2] = 0;
			numOfSelected_s = -999;
			numOfSelected_s2 = -999;
		}
		wx = 0;
		wy = 0;
		wz = 0;
		saveCount++;
		if(saveCount==3){
			saveCount=0;
		}
	}

	glutPostRedisplay();
}
#pragma endregion

void animate(void){
	glutPostRedisplay();
}

void myGlutIdle(){
	// 左の窓を再描画
	glutSetWindow(window1);
	glutPostRedisplay();
}


#pragma region // --- 文字列描画 ---
void DrawString(string str, int w, int h, int x0, int y0)
{
    glDisable(GL_LIGHTING);
    // 平行投影にする
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, w, h, 0);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
 
    // 画面上にテキスト描画
    glRasterPos2f(x0, y0);
    int size = (int)str.size();
    for(int i = 0; i < size; ++i){
        char ic = str[i];
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18 , ic);
    }
 
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

#pragma endregion


#pragma region // --- 立体の描画 ---
/// //////////////////////////////////////////
/// 球の描画
void drowSphere(double r, 
                double x, double y, double z, 
                MaterialStruct color){
	//球
	glPushMatrix();
      glMaterialfv(GL_FRONT, GL_AMBIENT, color.ambient);
      glMaterialfv(GL_FRONT, GL_DIFFUSE, color.diffuse);
      glMaterialfv(GL_FRONT, GL_SPECULAR, color.specular);
      glMaterialfv(GL_FRONT, GL_SHININESS, &color.shininess);
    glTranslated( x, y, z);//平行移動値の設定
	glutSolidSphere(r, 3, 3);//引数：(半径, Z軸まわりの分割数, Z軸に沿った分割数)
	glPopMatrix();
}

/// //////////////////////////////////////////
/// 正十二面体の描写
void drowDodecahedron(double r, 
                double x, double y, double z, 
                MaterialStruct color){
	//球
	glPushMatrix();
      glMaterialfv(GL_FRONT, GL_AMBIENT, color.ambient);
      glMaterialfv(GL_FRONT, GL_DIFFUSE, color.diffuse);
      glMaterialfv(GL_FRONT, GL_SPECULAR, color.specular);
      glMaterialfv(GL_FRONT, GL_SHININESS, &color.shininess);
    glTranslated( x, y, z);//平行移動値の設定
	//
	//glutSolidDodecahedron(r);
	glPopMatrix();
}

/// //////////////////////////////////////////
/// 直方体の描画
void drowCuboid(double a, double b, double c, 
                double x, double y, double z, 
                MaterialStruct color){

  GLdouble vertex[][3] = {
      { -a/2.0, -b/2.0, -c/2.0 },
      {  a/2.0, -b/2.0, -c/2.0 },
      {  a/2.0,  b/2.0, -c/2.0 },
      { -a/2.0,  b/2.0, -c/2.0 },
      { -a/2.0, -b/2.0,  c/2.0 },
      {  a/2.0, -b/2.0,  c/2.0 },
      {  a/2.0,  b/2.0,  c/2.0 },
      { -a/2.0,  b/2.0,  c/2.0 }
    };
  int face[][4] = {//面の定義
      { 3, 2, 1, 0 },
      { 1, 2, 6, 5 },
      { 4, 5, 6, 7 },
      { 0, 4, 7, 3 },
      { 0, 1, 5, 4 },
      { 2, 3, 7, 6 }
    };
  GLdouble normal[][3] = {//面の法線ベクトル
    { 0.0, 0.0, -1.0 },
    { 1.0, 0.0, 0.0 },
    { 0.0, 0.0, 1.0 },
    {-1.0, 0.0, 0.0 },
    { 0.0,-1.0, 0.0 },
    { 0.0, 1.0, 0.0 }
  };
    glPushMatrix();
      glMaterialfv(GL_FRONT, GL_AMBIENT, color.ambient);
      glMaterialfv(GL_FRONT, GL_DIFFUSE, color.diffuse);
      glMaterialfv(GL_FRONT, GL_SPECULAR, color.specular);
      glMaterialfv(GL_FRONT, GL_SHININESS, &color.shininess);
      glTranslated( x, y, z);//平行移動値の設定
      glBegin(GL_QUADS);
      for (int j = 0; j < 6; ++j) {
        glNormal3dv(normal[j]); //法線ベクトルの指定
        for (int i = 0; i < 4; ++i) {
          glVertex3dv(vertex[face[j][i]]);
        }
      }
      glEnd();
    glPopMatrix();
}


/// //////////////////////////////////////////
/// 回転を考慮した立方体の描画
void drowCuboid(double a, double b, double c, 
                double x, double y, double z, 
                MaterialStruct color, 
                double theta, 
                double nx, double ny, double nz){
  double nn =sqrt(pow(nx,2)+pow(ny,2)+pow(nz,2));
  if(nn>0.0){
    nx = nx/nn;
    ny = ny/nn;
    nz = nz/nn;
  }
  glPushMatrix();
    glTranslated( x, y, z);//平行移動値の設定
    glPushMatrix();
      if(theta!=0 && nn>0.0) glRotated( theta , nx , ny, nz);
      drowCuboid(a, b, c, 0, 0, 0, color);
    glPopMatrix();
  glPopMatrix();
}

#pragma endregion