/**
使用FFMpeg可以很方便的对音视频进行编码，并且写文件。

下面的代码是将5幅1280*720大小的图片进行编码，并且写到文件中。

代码有些乱，但希望能抛砖引玉，对学习这方面的朋友有帮助。
*/

CFile file[5];
BYTE *szTxt[5];

int nWidth = 0;
int nHeight= 0;

int nDataLen=0;

int nLen;

CString csFileName;
for (int fileI = 1; fileI <= 5; fileI ++)
{
  csFileName.Format("e:\\pics\\%d.bmp", fileI);
  file[fileI - 1].Open(csFileName,CFile::modeRead | CFile::typeBinary);
  nLen = file[fileI - 1].GetLength();

  szTxt[fileI -1] = new BYTE[nLen];
  file[fileI - 1].Read(szTxt[fileI - 1], nLen);
  file[fileI - 1].Close();

  //BMP bmi;//BITMAPINFO bmi;
  //int nHeadLen = sizeof(BMP);
  BITMAPFILEHEADER bmpFHeader;
  BITMAPINFOHEADER bmpIHeader;
  memcpy(&bmpFHeader,szTxt[fileI -1],sizeof(BITMAPFILEHEADER));

  int nHeadLen = bmpFHeader.bfOffBits - sizeof(BITMAPFILEHEADER);
  memcpy(&bmpIHeader,szTxt[fileI - 1]+sizeof(BITMAPFILEHEADER),nHeadLen);

nWidth = bmpIHeader.biWidth;// 464;// bmi.bmpInfo.bmiHeader.biWidth;// ;
  nHeight = bmpIHeader.biHeight;//362;// bmi.bmpInfo.bmiHeader.biHeight;// ;

  szTxt[fileI - 1] += bmpFHeader.bfOffBits;
  nDataLen = nLen-bmpFHeader.bfOffBits;
}

av_register_all();
avcodec_register_all();
AVFrame *m_pRGBFrame =  new AVFrame[1];  //RGB帧数据
AVFrame *m_pYUVFrame = new AVFrame[1];;  //YUV帧数据
AVCodecContext *c= NULL;
AVCodecContext *in_c= NULL;
AVCodec *pCodecH264; //编码器
uint8_t * yuv_buff;//

//查找h264编码器
pCodecH264 = avcodec_find_encoder(CODEC_ID_H264);
if(!pCodecH264)
{
  fprintf(stderr, "h264 codec not found\n");
  exit(1);
}

c= avcodec_alloc_context3(pCodecH264);
c->bit_rate = 3000000;// put sample parameters
c->width =nWidth;//
c->height = nHeight;//

// frames per second
AVRational rate;
rate.num = 1;
rate.den = 25;
c->time_base= rate;//(AVRational){1,25};
c->gop_size = 10; // emit one intra frame every ten frames
c->max_b_frames=1;
c->thread_count = 1;
c->pix_fmt = PIX_FMT_YUV420P;//PIX_FMT_RGB24;

//av_opt_set(c->priv_data, /*"preset"*/"libvpx-1080p.ffpreset", /*"slow"*/NULL, 0);
//打开编码器
if(avcodec_open2(c,pCodecH264,NULL)<0)
  TRACE("不能打开编码库");

int size = c->width * c->height;

yuv_buff = (uint8_t *) malloc((size * 3) / 2); // size for YUV 420

//将rgb图像数据填充rgb帧
uint8_t * rgb_buff = new uint8_t[nDataLen];

//图象编码
int outbuf_size=100000;
uint8_t * outbuf= (uint8_t*)malloc(outbuf_size);
int u_size = 0;
FILE *f=NULL;
char * filename = "e:\\pics\\myData.h264";
f = fopen(filename, "wb");
if (!f)
{
  TRACE( "could not open %s\n", filename);
  exit(1);
}

//初始化SwsContext
SwsContext * scxt = sws_getContext(c->width,c->height,PIX_FMT_BGR24,c->width,c->height,PIX_FMT_YUV420P,SWS_POINT,NULL,NULL,NULL);

AVPacket avpkt;

//AVFrame *pTFrame=new AVFrame
for (int i=0;i<250;++i)
{

  //AVFrame *m_pYUVFrame = new AVFrame[1];

  int index = (i / 25) % 5;
  memcpy(rgb_buff,szTxt[index],nDataLen);

  avpicture_fill((AVPicture*)m_pRGBFrame, (uint8_t*)rgb_buff, PIX_FMT_RGB24, nWidth, nHeight);

  //将YUV buffer 填充YUV Frame
  avpicture_fill((AVPicture*)m_pYUVFrame, (uint8_t*)yuv_buff, PIX_FMT_YUV420P, nWidth, nHeight);

  // 翻转RGB图像
  m_pRGBFrame->data[0]  += m_pRGBFrame->linesize[0] * (nHeight - 1);
  m_pRGBFrame->linesize[0] *= -1;
  m_pRGBFrame->data[1]  += m_pRGBFrame->linesize[1] * (nHeight / 2 - 1);
  m_pRGBFrame->linesize[1] *= -1;
  m_pRGBFrame->data[2]  += m_pRGBFrame->linesize[2] * (nHeight / 2 - 1);
  m_pRGBFrame->linesize[2] *= -1;


  //将RGB转化为YUV
  sws_scale(scxt,m_pRGBFrame->data,m_pRGBFrame->linesize,0,c->height,m_pYUVFrame->data,m_pYUVFrame->linesize);

  int got_packet_ptr = 0;
  av_init_packet(&avpkt);
  avpkt.data = outbuf;
  avpkt.size = outbuf_size;
  u_size = avcodec_encode_video2(c, &avpkt, m_pYUVFrame, &got_packet_ptr);
  if (u_size == 0)
  {
   fwrite(avpkt.data, 1, avpkt.size, f);
  }
}

fclose(f);
delete []m_pRGBFrame;
delete []m_pYUVFrame;
delete []rgb_buff;
free(outbuf);
avcodec_close(c);
av_free(c);
