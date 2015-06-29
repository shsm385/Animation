/*
244541 嶋田裕 　最終課題

・起動した後に特に操作は必要はありません。
・アニメーションは終了しないタイプなので、１分程度みて評価してください。
・普通は大丈夫なはずなのですが110行目に書いてあるmsleep(3)は書いてないと最初の水面の色が真っ白になってしまうという現象が起こるので書いてあります。自分なりに調べた結果どこかの部分がオーバーフローしているのがではないかと思われます。またこの部分でprintf文などターミナルに負荷をかけることによって解決するということは分かりました。
 */
#include<stdio.h>
#include<math.h>
#include<eggx.h>
#include<unistd.h>
#include<stdlib.h>

#define MAXLENGTH 300 //円の最大半径
#define MAXNUMBER 100 //保持できる最大個数
#define ENSHUU M_PI //円周率
#define THICK 2 //葉脈を描く太さ

/*関数のプロトタイプ*/
void branch(void);//木の幹を描く
void rightleaf(void);//上側の葉っぱ
void underleaf(void);//下側の葉っぱ
void mainleaf(void);//一番上の葉っぱ
void surfacewater(void);//水面を描く関数
void background(void);//背景の関数
int drop(int droparray[]);//水滴を非同期で落とす関数
int dropX(int count);//水滴のx座標を調べる
int dropY(int count);//水滴のy座標を調べる

/*グローバル変数*/
int win;

int main(){
  int a, i;//カウンタ変数
  int phase[MAXNUMBER];//波の位相の配列
  int color[MAXNUMBER];//波の色の配列
  int droparray[MAXNUMBER];//落ちる水滴の配列
  float x;//円の中心x座標
  float y;//円の中心y座標
  float r[MAXNUMBER];//円の半径の配列
  float scale;//減衰のための係数
  float phasescale;//位相の減衰係数
  float sum;//波を足し合わせる関数
  int count;//カウント変数
  int offset;//波の相殺
  int flag;//フラグ変数

  win=gopen(800,800);/*画面表示*/
  winname(win,"final");/*題名*/
  phasescale = 0.25;
  flag = 0;

  /*水滴の描画*/
  for(a=0; a<=1; a++){
    for(i=0; i<=150; i++){
      background();//背景を描く関数
      if(a==1){
	newpen(win , 1);
	fillcirc(win, 270, 610, 6, 6);	/*葉っぱの先の水滴を描く*/
      }
      branch();//木の幹を書く関数
      rightleaf();//上側の葉っぱを描く関数
      underleaf();//下側の葉っぱを描く関数
      mainleaf();//メインの葉っぱを描く関数
      surfacewater();//水面を描く関数
      newpen(win, 1);
      fillcirc(win, 426-i, 761-i, 5, 5);  /*葉っぱの初期移動*/
      msleep(50);
    }
  }
  
  /*波の初期設定*/
  for(i=0; i < MAXNUMBER; i++){
    color[i] = 128;
    phase[i] = -1;
    droparray[i]=-1;
    r[i]=(MAXLENGTH / MAXNUMBER) * i;//円の半径初期化
  }

  x=300.0;
  y=300.0;
  
  layer(win, 0, 1); /*表示は 0 番、描画は 1 番レイヤーで*/
  /*波が非同期に落ちる*/
  while(1){
    background();//背景を描く関数
    /*葉っぱの先の水滴を描く*/
    newpen(win , 1);
    fillcirc(win, 270, 610, 6, 6);
    branch();//木の幹を描く関数
    rightleaf();//上側の葉っぱを描く関数
    underleaf();//下側の葉っぱを描く関数
    mainleaf();//メインの葉っぱを描く関数
    surfacewater();//水面を描く関数

    /*初期化*/
    color[0] = 0;
    sum = 0;
    count = 0;

    for(i=0; i < MAXNUMBER; i++){
      if(phase[i] >= 200){
	phase[i] = -1;
	flag = 1;
      }
      if(phase[i] != -1){
	scale = (MAXNUMBER - phase[i])*0.01;
	sum += 128*(sin(ENSHUU/MAXNUMBER-phasescale*phase[i])*scale);
	count++;
	msleep(3);
      }
    }
    
    if(count > 0){
      color[0] = sum / count + 128 - offset;
      if(flag == 1){
	offset += color[0] - color[1];
	flag = 0;
      } 
      if(offset > 0){
	offset--;
      }else if(offset < 0){
	offset++;
      }
    }


    if(color[0] == 0 || color[0] >= 255 ){
      color[0] = 128;
    }

    for(i=MAXNUMBER-1; i>0; i--){
      color[i]=color[i-1];
    }

    for(i=MAXNUMBER-1; i>=0; i--) {
      if( r[i] > 0.0 ) {
	newhsvcolor(win, 200, color[i], 255);
	fillcirc(win, x, y, r[i], r[i]); 
      }
    }

    if(drop(droparray) == 1){
      for(i=0; i<MAXNUMBER; i++){
	if(phase[i] == -1){
	  phase[i] = 0;
	  break;
	}
      }
    }

    for(i=0; i<=MAXNUMBER; i++){
      if(phase[i] != -1){
	phase[i]++;
      }
    }
    copylayer(win, 1, 0); /*レイヤー 1 番の内容を 0 にコピー*/
    msleep(40);
  }

  gclose(win);/*つける必要はないですけど開いたら閉じたかったのでつけました*/
 
 
  return 0;
}


/*木の幹を描く関数*/
void branch(void){
  float x1[5], x2[4],  x4[4], x6[4], x8[4];//x座標の配列
  float y1[5], y2[4],  y4[4], y6[4], y8[4];//y座標の配列
  
  
  /*主幹を描く*/
  newrgbcolor(win, 107, 74, 43);
  x1[0]=800.0;    y1[0]=435.0;
  x1[1]=611.0;    y1[1]=637.0;
  x1[2]=408.0;    y1[2]=787.0;
  x1[3]=616.0;    y1[3]=657.0;
  x1[4]=800.0;    y1[4]=464.0;
  
  fillpoly(win, x1, y1, 5, 0);

  /*葉っぱと幹のつなぎ(メイン部分)*/
  x2[0]=430.0;    y2[0]=769.0;
  x2[1]=423.0;    y2[1]=761.0;
  x2[2]=426.0;    y2[2]=761.0;
  x2[3]=436.0;    y2[3]=764.0;
  
  fillpoly(win, x2, y2, 4, 0);

  /*葉っぱのつなぎ(右上部分)*/
  x4[0]=586.0;    y4[0]=672.0;/*左下*/
  x4[1]=614.0;    y4[1]=672.0;/*右下*/
  x4[2]=616.0;    y4[2]=676.0;/*右上*/
  x4[3]=582.0;    y4[3]=675.0;/*左上*/
  
  fillpoly(win, x4, y4, 4, 0);

  /*葉っぱのつなぎ部分(右下部分)*/
  x6[0]=652.0;    y6[0]=617.0;//左上
  x6[1]=648.0;    y6[1]=588.0;//左下
  x6[2]=654.0;    y6[2]=589.0;//右下
  x6[3]=656.0;    y6[3]=613.0;//右上
  
  fillpoly(win, x6, y6, 4, 0);
  
  /*枝だけの部分*/
  x8[0]=725.0;    y8[0]=535.0;
  x8[1]=738.0;    y8[1]=553.0;
  x8[2]=743.0;    y8[2]=548.0;
  x8[3]=730.0;    y8[3]=534.0;
  
  fillpoly(win, x8, y8, 3, 0);
}

/*右上の葉っぱの描画関数*/
void rightleaf(void){
  float x5[29], y5[29];//座標の配列
  
  /*葉っぱ部分*/
  newhsvcolor(win, 122, 255, 200);

  x5[0]=616.0;    y5[0]=676.0;
  x5[1]=618.0;    y5[1]=679.0;
  x5[2]=628.0;    y5[2]=679.0;
  x5[3]=645.0;    y5[3]=692.0;
  x5[4]=676.0;    y5[4]=696.0;
  x5[5]=675.0;    y5[5]=692.0;
  x5[6]=690.0;    y5[6]=693.0;
  x5[7]=693.0;    y5[7]=690.0;
  x5[8]=690.0;    y5[8]=684.0;
  x5[9]=699.0;    y5[9]=680.0;
  x5[10]=706.0;    y5[10]=674.0;
  x5[11]=717.0;    y5[11]=667.0;
  x5[12]=733.0;    y5[12]=653.0;
  x5[13]=743.0;    y5[13]=633.0;
  x5[14]=747.0;    y5[14]=626.0;
  x5[15]=747.0;    y5[15]=619.0;
  x5[16]=741.0;    y5[16]=617.0;
  x5[17]=722.0;    y5[17]=617.0;
  x5[18]=721.0;    y5[18]=615.0;
  x5[19]=692.0;    y5[19]=617.0;
  x5[20]=691.0;    y5[20]=613.0;
  x5[21]=686.0;    y5[21]=612.0;
  x5[22]=685.0;    y5[22]=615.0;
  x5[23]=679.0;    y5[23]=619.0;
  x5[24]=670.0;    y5[24]=621.0;
  x5[25]=663.0;    y5[25]=629.0;
  x5[26]=639.0;    y5[26]=646.0;
  x5[27]=628.0;    y5[27]=661.0;
  x5[28]=614.0;    y5[28]=672.0;
  
  fillpoly(win, x5, y5, 29, 0);
  
  /*葉脈を描く*/
  newlinewidth(win, THICK);//葉脈の太さ
  newhsvcolor(win, 122, 153, 255);//色指定
  
  moveto(win, 614, 673);
  lineto(win, 638, 672);
  lineto(win, 680, 673);
  
  moveto(win, 652, 671);/*主となる部分*/
  lineto(win, 684, 663);
  lineto(win, 689, 659);
  lineto(win, 705, 647);
  lineto(win, 717, 640);
  lineto(win, 727, 637);
  lineto(win, 731, 632);
  lineto(win, 739, 627);
  
  moveto(win, 689, 659);/*中上*/
  lineto(win, 702, 659);
  lineto(win, 710, 657);
  
  moveto(win, 667, 665);/*下１*/
  lineto(win, 674, 662);
  lineto(win, 680, 638);
    
  moveto(win, 705, 645);/*下2*/
  lineto(win, 715, 625);  
}

/*下の葉っぱを描く関数*/
void underleaf(void){
  float x7[49], y7[49];//座標の配列

  /*葉っぱ部分*/
  newhsvcolor(win, 122, 255, 200);//色指定

  x7[0]=648.0;    y7[0]=588.0;
  x7[1]=640.0;    y7[1]=585.0;
  x7[2]=636.0;    y7[2]=580.0;
  x7[3]=628.0;    y7[3]=572.0;
  x7[4]=618.0;    y7[4]=570.0;
  x7[5]=609.0;    y7[5]=562.0;
  x7[6]=606.0;    y7[6]=555.0;
  x7[7]=594.0;    y7[7]=527.0;
  x7[8]=589.0;    y7[8]=519.0;
  x7[9]=587.0;    y7[9]=513.0;
  x7[10]=588.0;    y7[10]=504.0;
  x7[11]=585.0;    y7[11]=503.0;
  x7[12]=587.0;    y7[12]=490.0;
  x7[13]=588.0;    y7[13]=481.0;
  x7[14]=593.0;    y7[14]=472.0;
  x7[15]=595.0;    y7[15]=466.0;
  x7[16]=604.0;    y7[16]=466.0;
  x7[17]=606.0;    y7[17]=468.0;
  x7[18]=607.0;    y7[18]=453.0;
  x7[19]=625.0;    y7[19]=431.0;
  x7[20]=642.0;    y7[20]=418.0;
  x7[21]=646.0;    y7[21]=415.0;
  x7[22]=648.0;    y7[22]=412.0;
  x7[23]=658.0;    y7[23]=412.0;
  x7[24]=658.0;    y7[24]=408.0;
  x7[25]=689.0;    y7[25]=410.0;
  x7[26]=692.0;    y7[26]=415.0;
  x7[27]=692.0;    y7[27]=456.0;
  x7[28]=688.0;    y7[28]=459.0;
  x7[29]=695.0;    y7[29]=477.0;
  x7[30]=696.0;    y7[30]=470.0;
  x7[31]=699.0;    y7[31]=470.0;
  x7[32]=698.0;    y7[32]=494.0;
  x7[33]=701.0;    y7[33]=500.0;
  x7[34]=704.0;    y7[34]=499.0;
  x7[35]=707.0;    y7[35]=496.0;
  x7[36]=712.0;    y7[36]=496.0;
  x7[37]=712.0;    y7[37]=507.0;
  x7[38]=706.0;    y7[38]=515.0;
  x7[39]=705.0;    y7[39]=522.0;
  x7[40]=700.0;    y7[40]=533.0;
  x7[41]=700.0;    y7[41]=543.0;
  x7[42]=691.0;    y7[42]=558.0;
  x7[43]=677.0;    y7[43]=567.0;
  x7[44]=673.0;    y7[44]=574.0;
  x7[45]=669.0;    y7[45]=584.0;
  x7[46]=662.0;    y7[46]=587.0;
  x7[47]=651.0;    y7[47]=587.0;
  x7[48]=654.0;    y7[48]=589.0;
  
  fillpoly(win, x7, y7, 49, 0);
  
  /*葉脈を描く*/
  newlinewidth(win, THICK);//葉脈の太さ指定
  newhsvcolor(win, 122, 153, 255);//色指定
  
  moveto(win, 650, 587);/*主となる部分(左下まで)*/
  lineto(win, 650, 574);
  lineto(win, 650, 553);
  lineto(win, 650, 519);
  lineto(win, 649, 491);
  lineto(win, 647, 458);
  lineto(win, 643, 445);
  
  moveto(win, 650, 574);/*左上１*/
  lineto(win, 645, 565);
  
  moveto(win, 650, 553);/*左上２*/
  lineto(win, 640, 524);
  lineto(win, 606, 482);
    
  moveto(win, 650, 519);/*右上２*/
  lineto(win, 661, 508);
  lineto(win, 665, 496);
  
  moveto(win, 649, 491);/*右下３*/
  lineto(win, 656, 483);
  lineto(win, 659, 468);
  lineto(win, 666, 457);
  lineto(win, 675, 439);
  
  moveto(win, 654, 568);/*右上１*/
  lineto(win, 662, 553);
  lineto(win, 667, 548);
  lineto(win, 677, 536);
  lineto(win, 689, 516);
}

/*水滴が落ちる部分の葉っぱ(一番上)*/
void mainleaf(void){
  float x3[41], y3[41];//座標の配列

  /*葉っぱ部分*/
  newhsvcolor(win, 122, 255, 200);//色指定
  x3[0]=416.0;    y3[0]=767.0;
  x3[1]=419.0;    y3[1]=763.0;
  x3[2]=352.0;    y3[2]=759.0;
  x3[3]=288.0;    y3[3]=718.0;
  x3[4]=312.0;    y3[4]=719.0;
  x3[5]=307.0;    y3[5]=711.0;
  x3[6]=305.0;    y3[6]=707.0;
  x3[7]=303.0;    y3[7]=696.0;
  x3[8]=300.0;    y3[8]=695.0;
  x3[9]=300.0;    y3[9]=692.0;
  x3[10]=286.0;    y3[10]=661.0;
  x3[11]=281.0;    y3[11]=657.0;
  x3[12]=281.0;    y3[12]=651.0;
  x3[13]=277.0;    y3[13]=647.0;
  x3[14]=272.0;    y3[14]=628.0;
  x3[15]=271.0;    y3[15]=624.0;
  x3[16]=270.0;    y3[16]=613.0;
  x3[17]=275.0;    y3[17]=613.0;
  x3[18]=293.0;    y3[18]=613.0;
  x3[19]=295.0;    y3[19]=615.0;
  x3[20]=305.0;    y3[20]=615.0;
  x3[21]=342.0;    y3[21]=627.0;
  x3[22]=343.0;    y3[22]=621.0;
  x3[23]=370.0;    y3[23]=635.0;
  x3[24]=383.0;    y3[24]=636.0;
  x3[25]=399.0;    y3[25]=647.0;
  x3[26]=418.0;    y3[26]=669.0;
  x3[27]=421.0;    y3[27]=676.0;
  x3[28]=423.0;    y3[28]=671.0;
  x3[29]=427.0;    y3[29]=679.0;
  x3[30]=428.0;    y3[30]=686.0;
  x3[31]=429.0;    y3[31]=688.0;
  x3[32]=428.0;    y3[32]=702.0;
  x3[33]=431.0;    y3[33]=705.0;
  x3[34]=434.0;    y3[34]=709.0;
  x3[35]=434.0;    y3[35]=718.0;
  x3[36]=427.0;    y3[36]=731.0;
  x3[37]=428.0;    y3[37]=753.0;
  x3[38]=432.0;    y3[38]=748.0;
  x3[39]=432.0;    y3[39]=759.0;
  x3[40]=424.0;    y3[40]=763.0;
  
  fillpoly(win, x3, y3, 41, 0);
  
  /*葉脈を描く*/
  newlinewidth(win, THICK);//太さの指定
  newhsvcolor(win, 122, 153, 255);//色指定
  
  moveto(win, 428, 761);/*主となる部分*/
  lineto(win, 416, 749);
  lineto(win, 402, 739);
  lineto(win, 394, 732);
  lineto(win, 383, 723);
  lineto(win, 361, 698);
  lineto(win, 357, 692);
  lineto(win, 325, 660);
  lineto(win, 319, 651);
  lineto(win, 315, 644);
  lineto(win, 289, 626);
  
  moveto(win, 402, 739);/*左上部分*/
  lineto(win, 391, 736);
  lineto(win, 369, 731);
  lineto(win, 352, 730);
  
  moveto(win, 394, 732);/*右上部分*/
  lineto(win, 389, 686);
  
  moveto(win, 361, 698);/*左中部分*/
  lineto(win, 342, 691);
  lineto(win, 327, 686);
  lineto(win, 313, 685);
  
  moveto(win, 357, 692);/*右中部分*/
  lineto(win, 351, 649);
  
  moveto(win, 319, 651);/*左下部分*/
  lineto(win, 299, 646);
  lineto(win, 287, 646);
  
  moveto(win, 315, 644);/*右下部分*/
  lineto(win, 309, 632);
  lineto(win, 307, 623);
}

/*水面の描画する関数*/
void surfacewater(void){
  newhsvcolor(win, 200, 128, 255);//色指定
  fillcirc(win, 300, 300, 300, 300); 
}

/*水滴を落とす関数*/
int drop(int droparray[]){
  int d;//カウンタ変数
  int flag;//フラグ変数

  flag = 0;//フラグ

  /*ランダムに雫が発生*/
  if(rand() % 100 == 0){
    for(d=0; d<MAXNUMBER; d++){
      if(droparray[d] == -1){
	droparray[d] = 0;
	break;
      }
    }
  }
  
  /*水滴の描画*/
  for(d=0; d<MAXNUMBER; d++){
    if(droparray[d] != -1){
      newpen(win, 1);
      fillcirc(win, dropX(droparray[d]), dropY(droparray[d]), 5, 5);
    }
  }

  /*各要素にインクリメントしてフラグをたてる*/
  for(d=0; d<MAXNUMBER; d++){
    if(droparray[d] != -1){
      droparray[d]++;
    }
    if(droparray[d] == 180){
      droparray[d] = -1;
      flag = 1;
    }
  }
  
  /*フラグ分岐*/
  if(flag == 1) return 1;
  else return 0;
  
}

/*水滴の落ちるX座標を調べる関数*/
int dropX(int count){
  if(count < 150){
    return 426 - count;
  }else{
    return 270 - (150 - count);
  }
}

/*水滴の落ちるY座標を調べる関数*/
int dropY(int count){
  if(count < 150){
    return 761 - count;
  }else{
    return 613 - ((613 - 300) / 30.0 * (count - 150));
  }
}

/*背景の関数*/
void background(void){
  newrgbcolor(win, 35, 71, 148); 
  fillrect(win, 0, 650, 800, 150);
  
  newrgbcolor(win, 188, 118, 60);  
  fillrect(win, 0, 0, 800, 650);
}
