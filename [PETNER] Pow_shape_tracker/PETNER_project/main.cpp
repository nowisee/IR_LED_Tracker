#include <cv.h>           
#include <highgui.h>      
#include "labeling.h"
#include <math.h>



int Find_Highest_Thresh(IplImage* img,int num_of_pix);
double ArcCalc(double lx1, double ly1, double lx2, double ly2);
float GetAngle(float x1, float y1, float x2, float y2);
float GetDist(float x1, float y1, float x2, float y2);
float GetToeAngle(float x1, float y1, float x2, float y2);

CvVideoWriter* VideoOut = NULL;

int WIDTH = 640;
int HEIGHT = 480;

float THRESH_PERCENT = 0.8; // 그레이 이미지 임계값 퍼센트
float THRESH_PERCENT_RED = 0.8;
float THRESH_PERCENT_GREEN = 0.95;
float THRESH_PERCENT_BLUE = 0.95;

// 히스토그램에서 최소한 num_of_pix_ 보다 많은 가장 높은 임계값을 추출
int num_of_pix_gray = 10;
int num_of_pix_r = 20;
int num_of_pix_g = 20;
int num_of_pix_b = 20;

unsigned char tmp_r = 0, tmp_g = 0, tmp_b = 0;

int frame_cnt = 0;
int id_arr[4] = {0};
int id;

IplImage* result_img;

void main()
{	
	result_img = cvCreateImage(cvSize(WIDTH*2, HEIGHT), 8, 3);

	CvFont font;
	cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX, 3.0, 3.0, 0, 3, CV_AA);

	CvScalar text_color;
	char* ID_TEXT = NULL;

	ID_TEXT = (char*)malloc(sizeof(char)*3);

	CBlobLabeling blob;
    
	IplImage *frame_1 = 0, *frame_2 = 0;
	IplImage *gray_image = cvCreateImage(cvSize(WIDTH, HEIGHT), 8, 1);
	IplImage *binary_image = cvCreateImage(cvSize(WIDTH, HEIGHT), 8, 1);

	IplImage* copy_image = cvCreateImage(cvSize(WIDTH, HEIGHT),8,3);
	IplImage* sub_intensity_image = cvCreateImage(cvSize(WIDTH, HEIGHT), 8, 3);

	IplImage* r_image = cvCreateImage(cvSize(WIDTH, HEIGHT), 8, 1);
	IplImage* g_image = cvCreateImage(cvSize(WIDTH, HEIGHT), 8, 1);
	IplImage* b_image = cvCreateImage(cvSize(WIDTH, HEIGHT), 8, 1);


	CvCapture* capture = cvCaptureFromFile("IR_alone_3.avi");
	CvCapture* capture_1 = cvCaptureFromFile("color_alone_3.avi");

	int key = 0;
	int row = 0, col = 0;


	////////////////////////////// 블랍 노이즈 제거용 ////////////////////////////
	float BLOB_RATIO_THRES = 0.6; // 블랍 전체에 백색 픽셀 비율을 임계값으로 함
	int LENGTH_WEIGHT = 5; // 블랍의 가로 또는 세로의 길이에 곱해지는 가중치로써 범위 내에 다른 블랍이 존재하는지 확인


	while(1)
	{                                      
		cvGrabFrame(capture);
		cvGrabFrame(capture_1);
		frame_1 = cvRetrieveFrame(capture);
		frame_2 = cvRetrieveFrame(capture_1);

		if(!frame_1 || key >= 27)
			break;

		cvCopy(frame_1,copy_image,0);


		/*
		cvZero(r_image);
		cvZero(g_image);
		cvZero(b_image);
		cvSplit(copy_image,b_image,g_image,r_image,0);


		int highest_intensity_red =  Find_Highest_Thresh(r_image,num_of_pix_r);
		int highest_intensity_green =  Find_Highest_Thresh(g_image,num_of_pix_g);
		int highest_intensity_blue =  Find_Highest_Thresh(b_image,num_of_pix_b);

		//printf("highest intensity : %d %d %d\n", highest_intensity_red,highest_intensity_green,highest_intensity_blue);
		
		int SUB_INTENSITY_R = (int)(highest_intensity_red * THRESH_PERCENT_RED) ; 
		int SUB_INTENSITY_G = (int)(highest_intensity_green * THRESH_PERCENT_GREEN);
		int SUB_INTENSITY_B = (int)(highest_intensity_blue * THRESH_PERCENT_BLUE);


			cvZero(sub_intensity_image);
		// 조건 1. 상기 논문처럼 '색상화(각 컬러에서 상수값의 명도값을 빼는 연산)" 을 통해 1차 노이즈 제거
		for(col = 0; col < HEIGHT; col++)
		{
			for(row = 0; row < WIDTH; row++)
			{
				int nIdx = col*gray_image->widthStep + row;
				int tIdx = col*frame_1->widthStep + row*frame_1->nChannels;

				tmp_r = (unsigned char)r_image->imageData[nIdx];
				tmp_g = (unsigned char)g_image->imageData[nIdx];
				tmp_b = (unsigned char)b_image->imageData[nIdx];
				
				if(tmp_r - SUB_INTENSITY_R >= 0)
					sub_intensity_image->imageData[tIdx + 2] = tmp_r - SUB_INTENSITY_R ;
		
				if(tmp_g - SUB_INTENSITY_G >= 0)
					sub_intensity_image->imageData[tIdx + 1] = tmp_g - SUB_INTENSITY_G ;
			
				if(tmp_b - SUB_INTENSITY_B >= 0)
					sub_intensity_image->imageData[tIdx + 0] = tmp_b - SUB_INTENSITY_B;

			}
		}




		cvShowImage("sub-intensity-video", sub_intensity_image);

		cvCvtColor(sub_intensity_image, gray_image, CV_BGR2GRAY);
		
		cvShowImage("gray-video", gray_image);


		// 최소한 10개 이상 있을때..
		int highest_intensity_gray =  Find_Highest_Thresh(gray_image,num_of_pix_gray);
		//printf("%d\n",(int)(highest_intensity_gray * THRESH_PERCENT));

		cvThreshold(gray_image, binary_image, (int)(highest_intensity_gray * THRESH_PERCENT), 255, CV_THRESH_BINARY);
		cvShowImage("binary-video", binary_image);
		*/

		cvCvtColor(frame_1, gray_image, CV_BGR2GRAY);
		cvThreshold(gray_image, binary_image, 240, 255,CV_THRESH_BINARY);

		cvDilate(binary_image,binary_image,0,2);
		cvErode(binary_image,binary_image,0,1);
	

		/*
		for(int row = 0; row < gray_image->height; row++)
			for(int col = 0; col < gray_image->width; col++)
				printf("%d ", (unsigned char)gray_image->imageData[col * gray_image->widthStep +row]);
		*/



		////////////////////////////////////////////// 여기부터 제어 시작!! /////////////////////////////////////////


		///////////////////////////// 먼저 발바닥 블랍 찾기!!! ////////////////////////////


		// 라벨링
		blob.SetParam(binary_image, 1); // 레이블링 할 영상과 최소 픽셀 수 
		blob.DoLabeling();

		// 발바닥 블랍 !!!!
		CvPoint pow_s_point = cvPoint(0,0), pow_e_point = cvPoint(0,0);
		CvPoint pow_cent_point = cvPoint(0,0);
		int max_area = 0;
		int pow_max_length = 0; // 발바닥의 가로, 세로 중 긴 길이
		CvPoint s_point= cvPoint(0,0), e_point= cvPoint(0,0);

		for(int i = 0; i < blob.m_nBlobs; i ++) 
		{
			s_point = cvPoint(blob.m_recBlobs[i].x,blob.m_recBlobs[i].y);
			e_point = cvPoint(blob.m_recBlobs[i].x + blob.m_recBlobs[i].width,blob.m_recBlobs[i].y + blob.m_recBlobs[i].height);
			
			// 블랍 탐지
			//cvRectangle(frame_1,s_point,e_point,cvScalar(255,255,0),2,8);
			

			/////////////////////// 노이즈 블랍 제거부 ///////////////////////
			int blob_width = e_point.x - s_point.x ;
			int blob_height = e_point.y - s_point.y ;


			//printf("b_w : %d, b_h : %d   ",  blob_width, blob_height);

			// 조건 1. 블랍 내 백색 픽셀의 비율(BLOB_RATIO_THRES = 0.7)을 통해 제거
			int sum = 0;
			float blob_ratio = 0.0f;

			for(col = s_point.y; col < e_point.y; col++)
			{
				for(row = s_point.x; row < e_point.x; row++)
				{
					int tmp_intens = (unsigned char)binary_image->imageData[col * binary_image->widthStep + row];
					if(tmp_intens == 255)
						sum++;
				}
			}
			
			if(sum == 0 || blob_height * blob_width == 0)
				continue;

			blob_ratio = (float)sum / (float)((blob_height) * (blob_width));

			//printf("%d / %d =  %.2f\n", sum, (blob_height) * (blob_width), blob_ratio);

			// 조건 2. 블랍 가로 세로 비율(발바닥 블랍은 가로세로 비율이 2를 넘지 않는다.
			float length_ratio = (blob_width > blob_height) ? blob_width/blob_height : blob_height/blob_width;

			// 조건 3. 블랍의 중점으로 부터 가로 세로 중 긴 길이의 3~4배 길이 내 적어도 2~3개의 블랍이 존재
			int max_length = (blob_width > blob_height) ? blob_width / 2 : blob_height / 2;
			//pow_max_length = max_length;

			CvPoint blob_center = cvPoint(s_point.x + blob_width / 2, s_point.y + blob_height / 2);

			CvPoint s_range, e_range;

			s_range.x = (blob_center.x - max_length * LENGTH_WEIGHT > 0) ? 
				blob_center.x - max_length * LENGTH_WEIGHT : 0;
			e_range.x = (blob_center.x + max_length * LENGTH_WEIGHT < binary_image->width ) ? 
				blob_center.x + max_length * LENGTH_WEIGHT : binary_image->width;

			s_range.y = (blob_center.y - max_length * LENGTH_WEIGHT > 0) ? 
				blob_center.y - max_length * LENGTH_WEIGHT : 0;
			e_range.y = (blob_center.y + max_length * LENGTH_WEIGHT < binary_image->height ) ? 
				blob_center.y + max_length * LENGTH_WEIGHT : binary_image->height;

			int inner_blob_cnt = 0;

			for(int j = 0; j < blob.m_nBlobs; j ++)
			{
				CvPoint inner_center_point = 
					cvPoint(blob.m_recBlobs[j].x + blob.m_recBlobs[j].width / 2 ,
					blob.m_recBlobs[j].y + blob.m_recBlobs[j].height / 2);
				
				if(inner_center_point.x >= s_range.x && inner_center_point.x <= e_range.x && 
					inner_center_point.y >= s_range.y && inner_center_point.y <= e_range.y)
					inner_blob_cnt++;
 
			}


			// 위 조건들을 만족하는 블랍만 표시
			if(blob_ratio > BLOB_RATIO_THRES && length_ratio <= 2.0 && inner_blob_cnt >= 2)
			{

				//cvRectangle(frame_1,s_point,e_point,cvScalar(0,255,255),2,8);
				// 조건 4. 상기 조건들을 만족하는 가장 큰 블랍
				if(max_area < blob_height * blob_width)
				{
					max_area = blob_height * blob_width;
					pow_s_point = s_point;
					pow_e_point = e_point;
				}
			}

		}// 발바닥 블랍 찾기 끝

		// 발바닥 중심
		pow_cent_point.x = pow_s_point.x + (pow_e_point.x - pow_s_point.x)/2;
		pow_cent_point.y = pow_s_point.y + (pow_e_point.y - pow_s_point.y)/2;

		// 발바닥 가로, 세로 중 긴 반지름
		pow_max_length = (pow_e_point.x - pow_s_point.x > pow_e_point.y - pow_s_point.y) ? 
			(pow_e_point.x - pow_s_point.x) / 2 : (pow_e_point.y - pow_s_point.y) / 2;

		cvRectangle(frame_1,pow_s_point,pow_e_point,cvScalar(0,0,255),2,8);



		// 발바닥 블랍을 찾은 후 일정 범위 내에서 전처리를 실시하는 이유는 영상 내 여러개의 LED 패치가 있을 수 있기 때문에..

		///////////////////////////// 레퍼런스 블랍 찾기!!! ////////////////////////////

		int REF_LENGTH_WEIGHT = 6;

		CvPoint ROI_s_point, ROI_e_point;

		ROI_s_point.x = (pow_cent_point.x - pow_max_length * REF_LENGTH_WEIGHT > 0) ? 
			pow_cent_point.x - pow_max_length * REF_LENGTH_WEIGHT : 0;
		ROI_e_point.x = (pow_cent_point.x + pow_max_length * REF_LENGTH_WEIGHT < binary_image->width ) ? 
			pow_cent_point.x + pow_max_length * REF_LENGTH_WEIGHT : binary_image->width;

		ROI_s_point.y = (pow_cent_point.y - pow_max_length * REF_LENGTH_WEIGHT > 0) ? 
			pow_cent_point.y - pow_max_length * REF_LENGTH_WEIGHT : 0;
		ROI_e_point.y = (pow_cent_point.y + pow_max_length * REF_LENGTH_WEIGHT < binary_image->height ) ? 
			pow_cent_point.y + pow_max_length * REF_LENGTH_WEIGHT : binary_image->height;

		cvRectangle(frame_1,ROI_s_point,ROI_e_point,cvScalar(255,0,255),1,8);

		/*
		IplImage * ROI_image = cvCreateImage(cvSize(ROI_e_point.x - ROI_s_point.x,ROI_e_point.y - ROI_s_point.y),8,1);
		cvZero(ROI_image);

		cvSetImageROI(binary_image,cvRect(ROI_s_point.x,ROI_s_point.y,ROI_e_point.x - ROI_s_point.x,ROI_e_point.y - ROI_s_point.y));
		cvCopy(binary_image,ROI_image,0);
		cvResetImageROI(binary_image);
		*/
		

		CBlobLabeling ROI_blob;

		CvPoint blob_s_point = cvPoint(0,0), blob_e_point = cvPoint(0,0), blob_cent_point = cvPoint(0,0);

		ROI_blob.SetParam(binary_image, 5); // 작은 노이즈는 제외한 블랍들만 추출
		ROI_blob.DoLabeling();

		//printf("%d ", ROI_blob.m_nBlobs);

		// 모든 후보 블랍들의 중심점 저장 
		CvPoint *candidates_cent_pt  = (CvPoint*)malloc(sizeof(CvPoint)*ROI_blob.m_nBlobs);
		float *angle = (float*)malloc(sizeof(float)*ROI_blob.m_nBlobs);

		for(int i = 0; i < ROI_blob.m_nBlobs; i++)
		{
			candidates_cent_pt[i].x = 0,candidates_cent_pt[i].y = 0;
			angle[i] = 1000; // 360를 넘는 각도로 초기화
		}

		// 조건에 만족하는 블랍의 개수를 파악
		int blob_cnt = 0;

	
		for(int i = 0; i < ROI_blob.m_nBlobs; i ++) 
		{

			blob_s_point = cvPoint(ROI_blob.m_recBlobs[i].x,ROI_blob.m_recBlobs[i].y);
			blob_e_point = cvPoint(ROI_blob.m_recBlobs[i].x + ROI_blob.m_recBlobs[i].width, ROI_blob.m_recBlobs[i].y + ROI_blob.m_recBlobs[i].height);

			blob_cent_point.x = ((blob_e_point.x - blob_s_point.x) / 2) +  blob_s_point.x;
			blob_cent_point.y = ((blob_e_point.y - blob_s_point.y) / 2) +  blob_s_point.y;


			// ROI 영역 외에 블랍의 중점이 있을 때 제외 (ROI 이미지를 만든 결과 영상의 흔들림으로 인한 문제 발생)
			if( blob_cent_point.x < ROI_s_point.x || blob_cent_point.x > ROI_e_point.x ||
				blob_cent_point.y < ROI_s_point.y || blob_cent_point.y > ROI_e_point.y)
				continue;

			// 발바닥 블랍은 제외
			if(blob_cent_point.x == pow_cent_point.x && blob_cent_point.y == pow_cent_point.y) 
				continue;


			// ROI 내에서 탐지되는 모든 블랍 표시
			cvRectangle(frame_1,blob_s_point,blob_e_point,cvScalar(0,0,255),1,8);



			// 조건 1. 블랍 내 백색 픽셀의 비율(BLOB_RATIO_THRES = 0.7)을 통해 제거
			int sum = 0;
			float blob_ratio = 0.0f;

			int blob_width = blob_e_point.x - blob_s_point.x ;
			int blob_height = blob_e_point.y - blob_s_point.y ;

			for(col = blob_s_point.y; col < blob_e_point.y; col++)
			{
				for(row = blob_s_point.x; row < blob_e_point.x; row++)
				{
					int tmp_intens = (unsigned char)binary_image->imageData[col * binary_image->widthStep + row];
					if(tmp_intens == 255)
						sum++;
				}
			}
			
			if(sum == 0 || blob_height * blob_width == 0)
				continue;

			blob_ratio = (float)sum / (float)((blob_height) * (blob_width));

			//printf("%d / %d =  %.2f\n", sum, (blob_height) * (blob_width), blob_ratio);

			// 조건 2. 블랍 가로 세로 비율(발바닥 블랍은 가로세로 비율이 2를 넘지 않는다.
			float length_ratio = (blob_width > blob_height) ? blob_width/blob_height : blob_height/blob_width;

			// 위 조건들을 만족하는 블랍만 표시
			if(blob_ratio > 0.6 && length_ratio <= 4.0)
			{
				// 조건에 만족하는 후보 블랍의 중심점들만 저장
				candidates_cent_pt[blob_cnt].x = blob_cent_point.x;
				candidates_cent_pt[blob_cnt].y = blob_cent_point.y;

				angle[blob_cnt] = GetAngle((float)pow_cent_point.x, (float)pow_cent_point.y, (float)blob_cent_point.x, (float)blob_cent_point.y);

				cvRectangle(frame_1,blob_s_point,blob_e_point,cvScalar(255,255,0),1,8);

				blob_cnt++;
			}
			
		}// end of 레퍼런스 블랍을 찾기 위한 블랍 반복문
		

		//cvWaitKey(0);

		// 저장된 후보 블랍들의 각도를 측정하여 최종 레퍼런스 블랍을 결정
		// 조건 1. 두 블랍의 각도는 거의 180 이다.

		float big_angle = 0.0f, small_angle = 0.0f, diff_angle = 0.0f;
		float min_diff_angle = 1000.0;
		int ref_idx_1 = 0, ref_idx_2 = 0;

		for(int i = 0; i < blob_cnt  ; i ++) 
		{
			for(int j = 0; j < blob_cnt ; j++)
			{
				if( angle[i] == angle[j])
					continue;

				big_angle = angle[i] > angle[j] ? angle[i] : angle[j];
				small_angle = angle[i] < angle[j] ? angle[i] : angle[j];

				diff_angle = abs(180.0 - (big_angle - small_angle));

				if(min_diff_angle > diff_angle)
				{
					min_diff_angle = diff_angle;
					ref_idx_1 = i;
					ref_idx_2 = j;
				}
			}
		}
		
		//printf("최소각 %f\n", min_diff_angle);

		// 최소각이 25도를 넘어갈 때? <---------------------------------- 처리 필요

		if(min_diff_angle > 25)
		{
			printf("Loosing ref pt!!\n");
			//cvWaitKey(0);
			//continue;

		}

		cvLine(frame_1,pow_cent_point,candidates_cent_pt[ref_idx_1],cvScalar(255,255,0),1,8,0);
		cvLine(frame_1,pow_cent_point,candidates_cent_pt[ref_idx_2],cvScalar(0,255,255),1,8,0);
		
	
		///////////////////////////////////////// inner ROI /////////////////////////////////////////////////

		// 레퍼런스 블랍 간의 중심점
		CvPoint ref_cent_point = cvPoint(0,0);

		//ref_cent_point.x = ((blob_e_point.x - blob_s_point.x) / 2) +  blob_s_point.x;
		//ref_cent_point.y = ((blob_e_point.y - blob_s_point.y) / 2) +  blob_s_point.y;

		ref_cent_point.x = (candidates_cent_pt[ref_idx_1].x > candidates_cent_pt[ref_idx_2].x) ?
			((candidates_cent_pt[ref_idx_1].x - candidates_cent_pt[ref_idx_2].x) / 2 ) + candidates_cent_pt[ref_idx_2].x :
		((candidates_cent_pt[ref_idx_2].x - candidates_cent_pt[ref_idx_1].x) / 2 ) + candidates_cent_pt[ref_idx_1].x;

		ref_cent_point.y = (candidates_cent_pt[ref_idx_1].y > candidates_cent_pt[ref_idx_2].y) ?
			((candidates_cent_pt[ref_idx_1].y - candidates_cent_pt[ref_idx_2].y) / 2 ) + candidates_cent_pt[ref_idx_2].y :
		((candidates_cent_pt[ref_idx_2].y - candidates_cent_pt[ref_idx_1].y) / 2 ) + candidates_cent_pt[ref_idx_1].y;

		
		cvLine(frame_1,candidates_cent_pt[ref_idx_1],candidates_cent_pt[ref_idx_2],cvScalar(0,0,255),1,8,0);
		
		//cvCircle(frame_1,pow_cent_point,1,cvScalar(0,0,255),2);
		//cvCircle(frame_1,ref_cent_point,1,cvScalar(255,0,0),2);
	



		float x1 =  candidates_cent_pt[ref_idx_1].x, x2 = candidates_cent_pt[ref_idx_2].x, 
			  y1 =  candidates_cent_pt[ref_idx_1].y, y2 = candidates_cent_pt[ref_idx_2].y;
		float m = 0.0f, b = 0.0f;

		CvPoint normal_vert_1 = cvPoint(0,0), normal_vert_2 = cvPoint(0,0);// 수직 좌표
		//CvPoint pow_normal_vert_1 = cvPoint(0,0), pow_normal_vert_2 = cvPoint(0,0);

		if(x1 == x2) // 기울기가 0 일때
		{
			normal_vert_1.x = x1;
			normal_vert_1.y = ROI_s_point.y;
			normal_vert_2.x = x2;
			normal_vert_2.y = ROI_e_point.y;


		}
		else // 기울기가 0이 아닐 때
		{
			
			//float y = (-1/m)*(float)(row - ref_cent_point.x)+ref_cent_point.y;
			//ROI_s_point 로 계산

			m = (y2 - y1)/(x2 - x1);

			// 우측 레퍼런스 포인트를 기준으로 각도를 획득하여 법선의 꼭지점의 x, y 좌표를 획득
			int right_ref_idx = (candidates_cent_pt[ref_idx_1].x < candidates_cent_pt[ref_idx_2].x) ? ref_idx_2 : ref_idx_1;

			float right_ref_angle = GetAngle((float)pow_cent_point.x, (float)pow_cent_point.y, (float)candidates_cent_pt[right_ref_idx].x, (float)candidates_cent_pt[right_ref_idx].y);

			//printf("%f \n", right_ref_angle);

			if((right_ref_angle >= 315.0 && right_ref_angle <= 360.0))
			{
				normal_vert_1.y = ROI_s_point.y;
				normal_vert_1.x = (int)(-m*normal_vert_1.y + ref_cent_point.x + ref_cent_point.y*m);

				normal_vert_2.y = ROI_e_point.y;
				normal_vert_2.x = (int)(-m*normal_vert_2.y + ref_cent_point.x + ref_cent_point.y*m);
			}
			else if((right_ref_angle >= 0.0 && right_ref_angle <= 45.0))
			{
				normal_vert_1.y = ROI_s_point.y;
				normal_vert_1.x = (int)(-m*normal_vert_1.y + ref_cent_point.x + ref_cent_point.y*m);

				normal_vert_2.y = ROI_e_point.y;
				normal_vert_2.x = (int)(-m*normal_vert_2.y + ref_cent_point.x + ref_cent_point.y*m);

			}
			else
			{
				normal_vert_1.x = ROI_s_point.x;
				normal_vert_1.y = (int)(-1/m * (normal_vert_1.x - ref_cent_point.x) + ref_cent_point.y);
				normal_vert_2.x = ROI_e_point.x;
				normal_vert_2.y = (int)(-1/m * (normal_vert_2.x - ref_cent_point.x) + ref_cent_point.y);

			}

		}
		cvLine(frame_1,normal_vert_1,normal_vert_2,cvScalar(0,0,255),1,8,0);


		//double ArcCalc(double lx1, double ly1, double lx2, double ly2);
		// 오류 제어(종종 음수 값이 나오거나 1000보다 큰 경우가 있음)
		if(normal_vert_1.x < 0 || normal_vert_1.y < 0 || normal_vert_2.x < 0 || normal_vert_2.y < 0 )
			continue;
		if(normal_vert_1.x > WIDTH || normal_vert_1.y > HEIGHT || normal_vert_2.x > WIDTH || normal_vert_2.y > HEIGHT )
			continue;


		

		// 발바닥 중심과 법선 벡터 사이의 사이각 구하기
		//...........................................
		
		
		// 블랍과 가까운 법선의 꼭지점 (회전을 위한 중심)
		CvPoint final_normal_vert = cvPoint(0,0);


		// 각 버튼 별 넘버링
		int numbering_1 = 0;
		int numbering_2 = 0;
		int numbering_3 = 0;
		int numbering_4 = 0;

		


		if(blob_cnt == 0)
		{
			printf("No blobs!!\n");

			numbering_1 = 0;
			numbering_2 = 0;
			numbering_3 = 0;
			numbering_4 = 0;

		}
		/*
		else if(blob_cnt == 4)
		{
			numbering_1 = 1;
			numbering_2 = 1;
			numbering_3 = 1;
			numbering_4 = 1;


		}
		*/
		else
		{

			float *toe_angle_array = (float*)malloc(sizeof(float)*blob_cnt);
			for(int i = 0; i < blob_cnt; i ++)
				toe_angle_array[i] = 0.0;

			numbering_1 = 0;
			numbering_2 = 0;
			numbering_3 = 0;
			numbering_4 = 0;

			int cnt = 0;
			
			// 레퍼런스 블랍이 아닌 첫 블랍의 인덱스

			while(cnt < blob_cnt)
			{
				if(cnt != ref_idx_1 && cnt != ref_idx_2)
					break;
				cnt++;
			}

			//float dist_1 = GetDist(pow_normal_vert_1.x, pow_normal_vert_1.y, candidates_cent_pt[cnt].x,candidates_cent_pt[cnt].y);
			//float dist_2 = GetDist(pow_normal_vert_2.x, pow_normal_vert_2.y, candidates_cent_pt[cnt].x,candidates_cent_pt[cnt].y);

			float dist_1 = GetDist(normal_vert_1.x, normal_vert_1.y, candidates_cent_pt[cnt].x,candidates_cent_pt[cnt].y);
			float dist_2 = GetDist(normal_vert_2.x, normal_vert_2.y, candidates_cent_pt[cnt].x,candidates_cent_pt[cnt].y);

			// 블랍에 가까운 법선의 꼭지점을 저장

			//printf("%f %f\n", dist_1, dist_2); 

			final_normal_vert = (dist_1 < dist_2) ? normal_vert_1 : normal_vert_2;
			
			cvCircle(frame_1,final_normal_vert,2,cvScalar(0,255,255),2,8,0);

			// 법선의 꼭지점을 기준으로 회전
			// (1) 법선의 각도를 먼저 구하기
			float normal_angle = GetAngle((float)pow_cent_point.x, (float)pow_cent_point.y, (float)final_normal_vert.x, (float)final_normal_vert.y);

			//printf("법선의 각도 : %f\n", normal_angle);

			// 법선의 각도를 0도로 변환하여 toe blob들을 회전


			// toe angle 저장
			//float toe_angle_array[4] = {1000.0,1000.0,1000.0,1000.0,}; // 1000이라는 임의의 수를 입력하여 조건 제어(360도보다 큰 수)

			for(int i = 0; i < blob_cnt; i++)
			{
				//레퍼런스 블랍 제외
				if(i == ref_idx_1 || i == ref_idx_2)
					continue;
				// 발바닥 블랍 제외
				if(candidates_cent_pt[i].x == pow_cent_point.x && candidates_cent_pt[i].y == pow_cent_point.y)
					continue;


				float radian = normal_angle * 3.141592 / 180.0;
				//radian =  * radian;

				int rotated_normal_vec_x = (int)((final_normal_vert.x - ref_cent_point.x) * cos(radian) -
					(final_normal_vert.y - ref_cent_point.y) * sin(radian) + ref_cent_point.x);
				int rotated_normal_vec_y = (int)((final_normal_vert.x - ref_cent_point.x) * sin(radian) +
					(final_normal_vert.y - ref_cent_point.y) * cos(radian) + ref_cent_point.y);

				int rotated_x = (int)((candidates_cent_pt[i].x - ref_cent_point.x) * cos(radian) -
					(candidates_cent_pt[i].y - ref_cent_point.y) * sin(radian) + ref_cent_point.x);
				int rotated_y = (int)((candidates_cent_pt[i].x - ref_cent_point.x) * sin(radian) +
					(candidates_cent_pt[i].y - ref_cent_point.y) * cos(radian) + ref_cent_point.y);

				cvLine(frame_1,ref_cent_point,cvPoint(rotated_normal_vec_x,rotated_normal_vec_y),cvScalar(0,50,255),1,8,0);
				cvCircle(frame_1,cvPoint(rotated_x,rotated_y),2,cvScalar(0,50,255),3,8,0);


				float toe_angle =  GetToeAngle((float)rotated_normal_vec_x, (float)rotated_normal_vec_y, (float)rotated_x, (float)rotated_y);

				toe_angle_array[i] = toe_angle;

				/*
				if(toe_angle >= 150 && toe_angle < 160)
					numbering_1 = 1;
				if(toe_angle >= 160 && toe_angle < 175)
					numbering_2 = 1;
				if(toe_angle >= -175 && toe_angle < -160)
					numbering_3 = 1;
				if(toe_angle >= -160 && toe_angle < -150)
					numbering_4 = 1;
					*/

				//printf("%5f ",toe_angle);

			}

			int flag = 0; // 170 을 넘거나 -170 보다 작은 각도가 발생되면 flag = 1

			for(int i = 0; i < blob_cnt; i++)
			{
				if(toe_angle_array[i] >= 170 || toe_angle_array[i] <= -170)
				{
					flag = 1;
					break;
				}
			}
			if(flag == 1)
			{
				for(int i = 0; i < blob_cnt; i++)
				{
					if(toe_angle_array[i] > 0)
						toe_angle_array[i] -= 10;
					else
						toe_angle_array[i] += 10;
				}
			}


			for(int i = 0; i < blob_cnt; i++)
			{

				if(toe_angle_array[i] >= 140 && toe_angle_array[i] < 160)
					numbering_1 = 1;
	
				if(toe_angle_array[i] >= 160 && toe_angle_array[i] < 175)
					numbering_2 = 1;
		
				if(toe_angle_array[i] > -175 && toe_angle_array[i] <= -160)
					numbering_3 = 1;
		
				if(toe_angle_array[i] > -160 && toe_angle_array[i] <= -140)
					numbering_4 = 1;
			

				printf("%5f ",toe_angle_array[i]);
			}

			free(toe_angle_array);
	
		}

		int total_number = numbering_1 * 1 + numbering_2 * 2 + numbering_3 * 4 + numbering_4 * 8;

		printf("%d th puppy\n",total_number);

		id_arr[frame_cnt%4] = total_number;

		if(id_arr[0] == id_arr[1] && id_arr[1] == id_arr[2] && id_arr[2] == id_arr[3])
			id = total_number;
			


	
		text_color = cvScalar(0, 255, 0);
/*
		if(total_number == 0) text_color = cvScalar(255, 255, 125);
		else if(total_number == 1) text_color = cvScalar(255, 255, 125);
		else if(total_number == 2) text_color = cvScalar(255, 125, 255);
		else if(total_number == 3) text_color = cvScalar(255, 100, 0);*/

		itoa(id, ID_TEXT, 10);

		cvPutText(frame_2, ID_TEXT, cvPoint(pow_cent_point.x-90, pow_cent_point.y), &font, text_color);

		if(frame_1 == 0 || frame_2 == 0)
			continue;

		cvSetImageROI(result_img, cvRect(0, 0, WIDTH, HEIGHT));
		cvCopy(frame_1, result_img);

		cvSetImageROI(result_img, cvRect(WIDTH, 0, WIDTH, HEIGHT));
		cvCopy(frame_2, result_img);

		cvResetImageROI(result_img);
		//cvResetImageROI(frame_2);

		if(VideoOut == NULL)
            VideoOut = cvCreateVideoWriter( "result video.avi", CV_FOURCC('D', 'I', 'V', '3'), 15, cvGetSize(result_img), 1);

		cvWriteFrame(VideoOut, result_img);


		frame_cnt++;

		cvShowImage("result", result_img);
		/*cvShowImage("color_video",frame_2);
		cvShowImage("video", frame_1);*/
		

		key = cvWaitKey(33);

		free(candidates_cent_pt);
	}

	cvReleaseVideoWriter(&VideoOut);

	cvReleaseCapture(&capture);
	cvReleaseCapture(&capture_1);

	cvReleaseImage(&gray_image);
	cvReleaseImage(&binary_image);
	
	cvReleaseImage(&copy_image);
	cvReleaseImage(&sub_intensity_image );
	cvReleaseImage(&r_image );
	cvReleaseImage(&g_image );
	cvReleaseImage(&b_image );

	free(ID_TEXT);

	cvDestroyAllWindows();

}

float GetToeAngle(float x1, float y1, float x2, float y2)
{

	x2 -= x1;
	y2 -= y1;

	float angle = atan2(-1*y2, x2) * 180.0 / 3.141592;

	//float angle = atan2(-1*y2, x2) ;
	
	return angle;

}





float GetDist(float x1, float y1, float x2, float y2)
{
	float dist = 0;

	dist = sqrt(pow(x2 - x1,2) + pow(y2 - y1,2));

	return dist;

}




double ArcCalc(double lx1, double ly1, double lx2, double ly2)// 각도 계산하기 입력된 직선의 xy값 2개

{

    double inner = (lx1*lx2 + ly1*ly2);// 기본내적
    double i1=sqrt(lx1*lx1+ ly1*ly1); // 처음 직선의 노말라이즈 준비
    double i2=sqrt(lx2*lx2+ ly2*ly2); // 두번째 직선의 노말라이즈 준비

 
    lx1=(lx1/i1); // 각 요소를 단위 벡터로 변환한다.
    ly1=(ly1/i1);
    lx2=(lx2/i2);
    ly2=(ly2/i2);

 

    //위 과정을 거치면 결과적으로 계산된 두 직선의 크기는 1이면서 방향은 이전과 같은 단위벡터가 된다.

    inner =(lx1*lx2 + ly1*ly2); //다시 내적을 구한다.

    double result = acos(inner)*180.0/3.141592;

    // 아크 코사인을 통해 라디안을 구하고 그걸 각도로 변환하기 위해 180을 곱하고 파이로나눈다.

    // 사이각은 최대 0-180도 사이마 존재함으로 입력된 x값을 참조하여 360에 가까운 값으로 변환하는 과정을 거친다.

    //if(lx1 > lx2) result=360-result;//사이각을 구하기 위해서는 이 부분을 삭제.

    

    return result ; 

}




int Find_Highest_Thresh(IplImage* img,int num_of_pix)
{

	// (fail) 평균값을 기반으로 임계값을 정해볼까? 아무래도 LED는 높은 명도를 가지고 있을테니..(X : 전체 이미지 평균은 0)
	// (success) 히스토그램으로 나타내보자!!
	// -> 히스토그램으로 gray_image 내 밝기값들을 나타내고 최소 10개 이상의 밝기값을 갖는 최대 밝기를 획득한다.
	// -> 최대 밝기 * THRESH_PERCENT(=0.9) 를 임계값으로 획득하여 이진화를 시킨다.

	int hist[256] = {0,};
	int highest_intensity = 0;

	for(int col = 0; col < img->height; col++)
	{
		for(int row = 0; row < img->width; row++)
		{
			int idx = (unsigned char)img->imageData[col * img->widthStep + row];
			hist[idx]++;
		}
	}

	for(int i = 255; i >= 0; i--)
	{
		if(hist[i] > num_of_pix)
		{
			highest_intensity = i;
			break;
		}
	}

	return highest_intensity;
}

float GetAngle(float x1, float y1, float x2, float y2)
{

	x2 -= x1;
	y2 -= y1;

	float angle = atan2(-1*y2, x2) * 180.0 / 3.141592;

	if (0 > angle)
		angle += 360;
	
	return angle;

}




/*
		if(x1 != x2)
		{
			m = (y2 - y1)/(x2 - x1);

			for(int row = pow_s_point.x; row < pow_e_point.x; row++)
			{
				float y = (-1/m)*(float)(row - ref_cent_point.x)+ref_cent_point.y;

				if(y >=0 && y <= binary_image->height)
				{
					binary_image->imageData[(int)y * binary_image->widthStep + row] = 255;
				}
			}

			//cvLine(frame_1,vert_1,vert_2,cvScalar(0,0,255),1,8,0);
		}
		else
		{
			for(int row = 0; row < binary_image->height; row++)
				binary_image->imageData[row * binary_image->widthStep + ref_cent_point.x] = 255;

		}
		*/
