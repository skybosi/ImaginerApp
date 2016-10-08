//
// Created by skybosi on 2016/10/7.
//

#include "android_ImgSdk_Imaginer.h"
#include "ipoint.h"
#include "dataPcerImp.h"
#include "dataPcer.h"

dataPcerImp* 	cimageObj = NULL;
dataPcer*       dpcer = NULL;

/*
 * Class:     android_ImgSdk_Imaginer
 * Method:    init
 * Signature: ([III)Z
 */
JNIEXPORT jboolean JNICALL Java_android_ImgSdk_Imaginer_init(JNIEnv * env, jobject obj, jintArray imageData, jint width, jint height)
{
    LOGD("imageData lenth:%d\tWidth:%d\t Height:%d\n",env->GetArrayLength(imageData), width,height);
    cimageObj = new dataPcerImp(width,height);
    PIXELS cpixel;
    jint *cbuf = env->GetIntArrayElements(imageData,NULL);
    if (cbuf == NULL) {
        LOGE("GetIntArrayElements in imageData error\n");
        return false; /* exception occurred */
    }
    try
    {
        for(int y = 0;y < height;++y)
        {
            for(int x = 0;x < width;++x)
            {
                int tmp = cbuf[ y * width + x];
                cimageObj->insert(x,y,tmp);
            }
        }
    }catch (exception& e)
    {
        LOGE("Exception occur:%s\n",e.what());
    }
    env->ReleaseIntArrayElements(imageData,cbuf,0);
    LOGD("init image is OOOOKKKKK!\n");
	dpcer = new dataPcer(cimageObj->getImageData(),cimageObj->getWidth(),cimageObj->getHeight());
	if(dpcer != NULL)
		LOGD("init Data Processing Center OOOOKKKKK!\n");
    return true;
}
/*
 * Class:     android_ImgSdk_Imaginer
 * Method:    getBoundrys
 * Signature: ()[[[I
 */
JNIEXPORT jobjectArray JNICALL Java_android_ImgSdk_Imaginer_getBoundrys(JNIEnv *env, jobject obj)
{
	LOGD("will be get All Boundrys...\n");
	if(dpcer != NULL)
	{
		LOGD("dealManager tp get Boundrys OK\n");
		dpcer->dealManager("g");
		LOGD("dealManager tp get Boundrys OK\n");
		vdPIXELS BoundrysData = dpcer->getBoundrysData();
		dPIXELS   boundry;
		int bsize = BoundrysData.size();
		LOGD("Boundrys size is:%d\n",bsize);
		int sizeAb = 0;
			
		//获得二维数组的类  
		jclass intArrCls2 = env->FindClass("[[I");
		
		//实例化数组对象，第一个参数数据的大小，第二个参数用来实例化用  
		//的类是一个二维数组，也就是数组里的每个元素都是一个二维数组，  
		//这样one就是一个三维数组。  
		jobjectArray one = env->NewObjectArray(bsize, intArrCls2, NULL);
		LOGD("boundry size:%d\n",bsize);
		for(int i = 0; i < bsize; ++i)
		{
			//一维数组  
			jclass intArrCls = env->FindClass("[I");  
			boundry = BoundrysData[i];
			sizeAb = boundry.size();
			LOGD("i:%d \tcreat a int[] size is:%d\n",i,sizeAb);
			//创建一个有10个元素，每个元素的值是  一维数组的数组
			jobjectArray second = env->NewObjectArray(sizeAb, intArrCls, NULL);
			if(second !=NULL) {
				LOGD("new a jobjectArray size is:%d OK\n", sizeAb);
			}else
			{
				LOGD("new a jobjectArray size is:%d is fail\n", sizeAb);
			}
			//给以维数据填充值  
			for (int j = 0; j < sizeAb; ++j) 
			{  
				jint tmp[3];
				jintArray iarr = env->NewIntArray(3);  
				tmp[0] = boundry[j].getX();
				tmp[1] = boundry[j].getY();
				tmp[2] = boundry[j].getEdge();
				//LOGD("tmp pixle is x:%d  y:%d  edge:%d\n",tmp[0],tmp[1],tmp[2]);
				//把temp里的0-3的数据值设置给iarr里  
				env->SetIntArrayRegion(iarr, 0, 3, tmp);  
				//给一维数组的第i个元素设置值  
				env->SetObjectArrayElement(second, j, iarr);  
				//删除临时元素iarr数组  
				env->DeleteLocalRef(iarr);  
			}  
			//给三维数组里的每个元素设置值，值是一个有一个元素组成的二维数据，每个元素是一个由三个整形小数组成的三维数组  
			env->SetObjectArrayElement(one, i, second);  
			//删除临时元素二维数组  
			env->DeleteLocalRef(second);  
		}  
		return one; 
	}
	else
		LOGE("Data Processing Center is vaild!\n");
	return NULL;
}


/*
 * Class:     android_ImgSdk_Imaginer
 * Method:    cfinalize
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_android_ImgSdk_Imaginer_cfinalize(JNIEnv *env, jobject obj)
{
	//will be use free all memory at c
/*
	if(cimageObj != NULL)
	{
		delete cimageObj;
		cimageObj = NULL;
		cimageData = NULL;
		prevDiret = Right;
	}
	*/

}
