//============================================================================
// Name        : main.cpp / intelligent
// Author      : khilmi@eros
// Version     : 8.0
// Copyright   : EROS Re-BORN Team
// Description : We Must Win This Competition, No Matter What !!
//============================================================================
#include "eheader.h"
using namespace std;

float radSudutY, jarakBola;


int main(int argc, char **argv){
	pthread_t threadsearching;
	struct timeval t1, t2;
	char system_check[100];
	double elapsedTime;
	//unsigned char param1,param2,param3;
	//int setX2,setY2,setX,setY,setH,eX,eY,eH;
	//int xpos2,ypos2;
	//int ct;

	//=====================Data kepala lihat==========
	//short sdtlihatkpl;

	cout << "Bismillah" << endl;
	
	//initplayer();
	
	if(impUSBRX() == 1){
		fprintf(stderr,".. USBRX.eros Loaded..\n");
	}
	else fprintf(stderr,".. USBRX.eros failed to load..\n");
	
	sprintf(system_check,"stty -F %s 57600",USBRX);
	fprintf(stderr,"%s\n\n",system_check);
	system(system_check);
	system(system_check);
	system(system_check);
	

	//Load Data Intialize ================================================
	if(impDatakpl() == 1){
		fprintf(stderr," Alhamdulillah data kalibrasi kepala berhasil di load\n");
	}
	else fprintf(stderr," Astaghfirullah data kalibrasi kepala berhasil di load\n");



	//ROS Publish/Subscribe Intialize ====================================
	ros::init(argc, argv, "intelligent");
	ros::NodeHandle n;

	//serial/serialcom.cpp
	ros::Subscriber serial_it = n.subscribe("serial_it", 100, SerialItCallback);
	ros::Publisher  it_serial = n.advertise<std_msgs::Int32MultiArray>("it_serial", 100);

	//referee/referee.cpp
	ros::Subscriber referee_it = n.subscribe("referee_it", 100, RefereeItCallback);

	//localization/localization.cpp
	ros::Subscriber vision_local = n.subscribe("vision_local", 100, VisionLocalCallback);

	//communication/clientrec.cpp
	ros::Subscriber rec_it = n.subscribe("rec_it", 100, RecItCallback);
	ros::Publisher  it_rec = n.advertise<std_msgs::Int32MultiArray>("it_rec", 100);

	//vision/vision.cpp
	//ros::Subscriber vision_it = n.subscribe("vision_it", 100, VisionItCallback);
	ros::Subscriber vision_it = n.subscribe("vision/outputs", 100, VisionItCallback);
	ros::Publisher  it_vision = n.advertise<std_msgs::Int32MultiArray>("it_vision", 100);

	ros::Publisher  it_rc = n.advertise<std_msgs::Float32>("robotcontrol/ai/heading", 100);
	ros::Publisher  it_headcontrol = n.advertise<head_control::LookAtTarget>("/robotcontrol/headcontrol/target", 1);
	ros::Publisher  it_debug = n.advertise<std_msgs::Int32MultiArray>("it_debug", 100);

	n.getParam("use_gui",use_gui);
	ROS_INFO("Set PID for GUI: [%d]",use_gui);

	n.getParam("debug_mode",debug_mode);
	ROS_INFO("Debugging Mode: [%d]",debug_mode);

	n.getParam("team",tim);
	n.getParam("player",player);

	pthread_create(&threadsearching,NULL,ftBall5,NULL);
	//pthread_create(&threadintelligent,NULL,intelligent,NULL);
	
	//Dummy
	//n.getParam("/position/xpos",setX);
	//n.getParam("/position/ypos",setY);
	//n.getParam("/position/hpos",setH);
	//setX=setY=setH=0;
	//ROS_INFO("Robot Position:[%d][%d][%d]",setX,setY,setH);

	//Load Parameter YAML ------------------------------------------------
	head_control::LookAtTargetPtr lookMsg = boost::make_shared<head_control::LookAtTarget>();
	


	//Nilai default variable =============================================
	//modePlay = 1;
	//modeTaktik = 2;
	//stepG = stepK = 1;
	step = stepTB = BackHome = stepH = 0;
	kaki = play =  0;
	flagsudahdekat=0;
	enablevision = 0b0001;
	lastXBall = sdtblax;
	lastYBall = sdtblay;
	//setX=-2250;setY=2250; setH=-90;
	//KNN_Basic(3,25,377,146,1);
	flagGetFieldSampleData=false;
	flagCuri=false;
	init();
	while (ros::ok()){
		//flagLocalize=1;
		//gettimeofday(&t1, NULL);
		//dtaYPOS=650;
		/*xpos2 = dtaXPOS*cosd(dtaHeading)-dtaYPOS*sind(dtaHeading);
		ypos2 = dtaXPOS*sind(dtaHeading)+dtaYPOS*cosd(dtaHeading);
		setX2 = setX * cosd(dtaHeading) - setY * sind(dtaHeading);
		setY2 = setX * sind(dtaHeading) + setY * cosd(dtaHeading);
		eX=setX2-xpos2;
		eY=setY2-ypos2;
		eH=dtaHeading-setH;
		
		//Fuzzyfikasi
		if(eY>700)			param1=40;
		else if(eY>400)	param1=30;
		else if(eY>100)	param1=20;
		else 						param1=10;
		
		if(eX>400)			param2=6;
		else if(eX>100)	param2=4;
		else if(eX<-400)param2=5;
		else if(eX<-100)param2=3;
		else						param2=0;
		
		if(eH>10)				param3=1;
		else if(eH<-10)	param3=2;
		else						param3=0;
		
		//Defuzzyfikasi
		if(param3==0){
			motion=param1+param2;
		}else{
			motion=param1+param3;
		}
		//if(motion==10)ct++;
		//if(ct>300){setX=0;setY=0;setH=0;}
		//if(motion==45)motion=35;
		//if(motion==46)motion=36;
		motionAct(2048,2048,motion,0);*/


		//MAIN LOOP ---------------------------------------------------------------------------------------------------------------------
		arahRobot = cariarahRobot(dtaHeading,0);
		HeadingRad.data=-1*dtaHeading*R2D;
		arahLihat = CariArahLihat(SudutBolaX(),0);
		lookMsg->vec.z=-1*sdtbolax*R2D-HeadingRad.data;
		lookMsg->vec.y=-1*sdtbolay*R2D;
		lookMsg->vec.x=0;
		lookMsg->enabled=true;
		lookMsg->is_angular_data=true;
		lookMsg->is_relative=false;
		lookMsg->pitchEffort=0;
		lookMsg->yawEffort=0;		
		//pitchK.data=sdtbolax*R2D;
		//yawK.data=sdtbolay*R2D;
		//bermain();
		//lostball();
		//lostball_bertahan();
		//lostball_keeping();
		//strategi_serang_WIDE3();
		//TaktikEksekusiSamping(2);
		//strategi_bertahan_cpp();
		//strategi_serang_rokh();
		//strategi_bertahan_cpp(); 
		//strategi_bertahan_off();
		//PositionGenerator();
		//aktifkansearching();
		//TaktikkeBola3(1);
		//TaktikLuruskanGW_wide();
		//arahNggiring=1;
		//TaktikNggiring3();
		//TaktikEksekusi4(); 
		//DEBUG STRATEGY
		//ROS_INFO("(%d)X(%d)Y(%d)sX(%d)sY(%d)m(%d)xB(%d)yB(%d)aR(%d)><(%d)",nBall,dataXB,dataYB,sdtbolax,sdtbolay,motion,xBall,yBall,arahRobot,cariarahRobot(dtaHeading,0));

		//PositioningKI();		
		//PositioningKA();
		//bermain();
		//lostball_keeping();
		//TaktikEksekusi4();
		//TaktikEksekusi_keeping();	
		//pinalti();	
		//strategi_keeping1();
		//lostball();
		//strategi_serang_WIDE3();
		//PositionGenerator();
		//aktifkansearching(); motionAct(xBall, yBall,0, 10);
		//TaktikkeBola3(1);
		//arahNggiring=1;
		//TaktikNggiring3();
		//DEBUG STRATEGY
		//regression();
		//Strategijatuh1();
		//ROS_INFO("(%d)X(%d)Y(%d)sX(%d)sY(%d)m(%d)xB(%d)yB(%d)aR(%d)",nBall,dataXB,dataYB,sdtbolax,sdtbolay,motion,xBall,yBall,arahRobot);
//////		//DEBUG TACTIC
		//motion = 0;
		fprintf(stderr,"n(%d)S(%d)m(%d)odoX(%d)odoY(%d)Ox(%1.2f)Oy(%1.2f)X(%d)Y(%d)aR(%d)aL(%d)dtaFall(%d)Pball(%1.1f)jrkbll(%d)CN(%d)\n",nBall,step,motion,dtaXPOS,dtaYPOS,posisiX,posisiY,dataXB,dataYB,arahRobot,arahLihat,dtaFall,probball,jrkbolay,countn);
		
		//ROS_INFO_THROTTLE(1,"n(%d)S(%d)m(%d)odoX(%d)odoY(%d)Ox(%1.2f)Oy(%1.2f)X(%d)Y(%d)aR(%d)aL(%d)dtaFall(%d)Pball(%1.1f)\n",nBall,step,motion,dtaXPOS,dtaYPOS,posisiX,posisiY,dataXB,dataYB,arahRobot,arahLihat,dtaFall,probball);	
		
		//fprintf(stderr,"n(%d) AR(%d) xBall = %4d yBall = %4d dataXB = %3d direktori = %d dataYB = %3d speedBall = %3d m = %d stepKeep (%d) regresx(%0.2f)regresy(%0.2f)\n",nBall,arahRobot,xBall,yBall,dataXB,direktori,dataYB,speedBall,motion,stepKeep,ynew1,ynew);
		//Generate Sample Data Positioning --------------------------
		//initpos=1;
		//GetFieldSampleData(1);
		//-----------------------------------------------------------

		//DEBUG TACTIC
		//fprintf(stderr,"YPOS(%d)S(%d)nBall(%d)Y(%d)\n",dtaYPOS,step,nBall,dataYB);
		//fprintf(stderr,"%d(%d)X(%3d)Y(%3d)sX(%3d)sY(%3d)m(%2d)aR(%2d)aL(%2d)X(%4d)Y(%4d)%d[%d]::%d::%d\n",step,nBall,dataXB,dataYB,sdtbolax,sdtbolay,motion,arahRobot,arahLihat,dtaXPOS,dtaYPOS,batasGiringY,arahTendang,dtaFall,dtflagsama);
		
		//DEBUG I/O
		//ROS_INFO("(%d)X(%3d)Y(%3d)sX(%2d)sY(%2d)m(%2d)xB(%4d)yB(%4d)aR(%2d)aL(%2d)\n",nBall,dataXB,dataYB,sdtbolax,sdtbolay,motion,xBall,yBall,arahRobot,arahLihat);
		//ROS_INFO("(%d)aR(%2d)aL(%2d)m(%2d)L(%d)mLS(%d)mLT(%d)dtJob(%d)",nBall,arahRobot,arahLihat,motion,langkah,majuloss,majulost,dtJob);
		//printf("Hasil = %d", sdtlihatkpl);
		

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//if(dtaYPOS>3000) step=-6;
		//bermain();
		//pinalti();
		//TaktikBacktoPos_keeping();
		//TaktikkeBola_keeping();
		//strategi_serang_keep();
		//lostball_keeping();
		//motion=20;
		//xBall=sdtblax;
		//yBall=sdtdefy+170;
		//lokalisasi();
		//TaktikEksekusi_keeping();
		strategi_keeping2();
		//aktifkansearching();
		//updatelokasi();
		//fieldloc();
		//regression();
//kanan
//		if(arahRobot<3 || arahRobot<20 && arahRobot<11) motion=12;
//		else if(arahRobot>3 && arahRobot<12)	motion=11;
//		else motion=20;

//kiri
//		if(arahRobot>19 || arahRobot>=1 && arahRobot<11) motion=11;
//		else if(arahRobot>10 && arahRobot<19)	motion=12;
//		else motion=20;
		//aktifkansearching();
		//motionAct(xBall,yBall,0,0);
		//motionAct(sdtblax,sdtexcute,0,0);

//		kalibrasi bola
//		if(dtaFall==50) motion=0;
//		else if(dtaFall==40) aktifkansearching();
//		if (dtaFall==51) {yBall=sdtexcute;}
//		else if (dtaFall==52) {yBall=sdtexcute+100;}
//		else if (dtaFall==53) {yBall=sdtdefy-100;}
//		else if (dtaFall==54) {yBall=sdtdefy;}
//		else if (dtaFall==55) {yBall=sdtdefy+100;}
//		else if (dtaFall==6) aktifkansearching();

//		if (dtaFall==51) yBall=sdtexcute;
//		else if (dtaFall==52) yBall=sdtexcute+100;
//		else if (dtaFall==53) yBall=sdtdefy-100;
//		else if (dtaFall==54) yBall=sdtdefy;
//		else if (dtaFall==55) yBall=sdtdefy+100;
//		else if (dtaFall==6) {
//														ydirectpos=1;
//														aktifkansearching();
//														if(arahRobot>11) motion=11;
//														else if(arahRobot<11)	motion=12;
//														else
//														{
//															if(posisiX<-3) 
//															{ 
//																if(posisiY<-1.9) motion=16;
//																else if(posisiY>1.9) motion=15;
//																else motion=0;
//															}
//															else motion=20;
//														}
//													}

//														if(arahRobot>11) motion=12;
//														else if(arahRobot>1)	motion=11;
//														else if(arahRobot==1)
//														{	if(dtaXPOS<-1425) motion=0;
//															else motion=15;}
//													}
		//motion=20 -> 950 (1meter)
		//motion=15 -> -475 (1meter)
		//motion=16 -> 475 (1meter)
//		else if (dtaFall==6) {
//														ydirectpos=1;
//														aktifkansearching();
//														if(arahRobot>11) motion=12;
//														else if(arahRobot>1)	motion=11;
//														else if(arahRobot==1)
//														{
//															if(posisiX>3)
//															{ 
//																if(posisiY<-1.9) motion=15;
//																else if(posisiY>1.9) motion=16;
//																else motion=0;
//															}
//															else motion=20;
//														}
//													}
//		else dtaFall=0;

// nyoba jatuh keeper		
//		if(motion==83 || motion==84) {motion=0; dtaFall=6;}
//		if (dtaFall==50) {motion=0; countn++; if(countn>0) motion=70;}
//		else if (dtaFall==40) {motion=0; countn++; if(countn>50) motion=71;}
//		else if (dtaFall==6) motion=0;

// nyoba locallization vision modul
//		if(posisiX>2.5) motion=0;
//		else motion=20;

// ambil data odoY
//		if(arahRobot>11) motion=12;
//		else if(arahRobot>1)	motion=11;
//		else if(arahRobot==1)
//		{
//			countn++;
//			if(countn>100)motion=20;
//			else motion=0;
//		}

//		motionAct(xBall,yBall,motion,0);
	
		if(dtaYPOS==0 && dtaXPOS==0) resetOdometry=false;
		VisionPublish(yBall,SudutBolaY(),arahLihat,dtJob,enablevision);
		SerialPublish(resetOdometry,1,0,0);
		RecPublish(sdtbolay, dtJob, penalty, arahRobot, arahLihat, nBall);
		DebugPublish(dtaFall,arahRobot,arahLihat,nBall,dtJob,refree);
		it_serial.publish(dtaPublishSERIAL);
		it_rec.publish(dtaPublishREC);
		it_vision.publish(dtaPublishVISION);
		it_headcontrol.publish(lookMsg);
		it_rc.publish(HeadingRad);
		it_debug.publish(dtaPublishDEBUG);
		ros::spinOnce();
		//gettimeofday(&t2, NULL);
		//elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000.0;      // sec to ms
		//elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0;   // us to ms
		//fprintf(stderr," Time : %f \n",elapsedTime);
	}
}

