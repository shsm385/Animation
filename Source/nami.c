#include<stdio.h>
#include<math.h>
#include<eggx.h>
#include<unistd.h>

#define MAXLENGTH 300 //円の最大半径
#define MAXNUMBER 100//保持できる最大個数
#define kakudo M_PI

int win;

int main(){
  int position;
  int i, j, m, color[MAXNUMBER];
  float x, y, r[MAXNUMBER], tempx, tempy, scale, phase;
  win=gopen(800,800);/*画面表示*/
  winname(win,"final");/*題名*/
  newpen(win, 3);
  phase=0.25;
  
  

  for(j=0; j<MAXNUMBER; j++){
    color[j]=255;
    r[j]=(MAXLENGTH / MAXNUMBER) * j;//円の半径初期化
  }
  position=0;//ポジションをゼロに
  
  tempx=350.0;
  tempy=400.0;
  m=0;
  /*水面の描画*/
  /*newpen(win, 4);
    fillcirc(win, 0, 400, 670, 700);*/
  
  /*水滴の描画*/
  /*for(i=0; i<=400; i=i+5){
    newpen(win, 1);
    fillcirc(win, 400, 780-i, 5, 5);
    newpen(win, 4);
    fillcirc(win, 400, 788-i, 6, 4);
    msleep(50);
    }*/
  while(1){
    scale=(100 - m % 100)*0.01;
    color[0]=128*(sin(kakudo/MAXNUMBER-phase*m)*scale+(1-scale))+128;
    x=tempx; y=tempy; // 値を保持
    position=(position+1) % MAXNUMBER; // ポジションを次に移動して待つ
    for(j=MAXNUMBER-1; j>0; j--){
      color[j]=color[j-1];
    }

    for(j=MAXNUMBER-1; j>=0; j--) {
      if( r[j] > 0.0 ) {
	newhsvcolor(win, 200, color[j], 255);
	fillcirc(win, x, y, r[j], r[j]); // 半径がゼロで無ければ描画
	if( r[j] > MAXLENGTH ) r[j]=0.0; // 大きくなりすぎたら中止
      }
     }

    m++;
    msleep(40);
   }
  
  
  ggetch();
  gclose(win);
 
  return 0;
}



//sin(kakudo*j/MAXNUMBER-0.5*m)*64+128+64 計算
