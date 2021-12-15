#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // sleep()関数を使う
#include <time.h>
#include <string.h>
#include "gol.h"

void my_init_cells(const int height, const int width, int cell[height][width], FILE* fp);
void my_print_cells(FILE* fp, int gen, const int height, const int width, int cell[height][width]);
void my_update_cells(const int height, const int width, int cell[height][width]);
int my_count_adjacent_cells(int h, int w, const int height, const int width, int cell[height][width]);
void put_cells(int gen, const int height, const int width, int cell[height][width]);

int main(int argc, char **argv)
{
  FILE *fp = stdout;
  const int height = 40;
  const int width = 70;

  int cell[height][width];
  for(int y = 0 ; y < height ; y++){
    for(int x = 0 ; x < width ; x++){
      cell[y][x] = 0;
    }
  }

  /* ファイルを引数にとるか、ない場合はデフォルトの初期値を使う */
  if ( argc > 2 ) {
    fprintf(stderr, "usage: %s [filename for init]\n", argv[0]);
    return EXIT_FAILURE;
  }
  else if (argc == 2) {
    FILE *lgfile;
    if ( (lgfile = fopen(argv[1],"r")) != NULL ) {
      my_init_cells(height,width,cell,lgfile); // ファイルによる初期化
    }
    else{
      fprintf(stderr,"cannot open file %s\n",argv[1]);
      return EXIT_FAILURE;
    }
    fclose(lgfile);
  }
  else{
    my_init_cells(height, width, cell, NULL); // デフォルトの初期値を使う
  }

  my_print_cells(fp, 0, height, width, cell); // 表示する
  sleep(1); // 1秒休止
  fprintf(fp,"\e[%dA",height+3);//height+3 の分、カーソルを上に戻す(壁2、表示部1)

  /* 世代を進める*/
  for (int gen = 1 ;; gen++) {
    my_update_cells(height, width, cell); // セルを更新
    my_print_cells(fp, gen, height, width, cell);  // 表示する
    put_cells(gen,height,width,cell);
    sleep(1); //1秒休止する
    fprintf(fp,"\e[%dA",height+3);//height+3 の分、カーソルを上に戻す(壁2、表示部1)
  }

  return EXIT_SUCCESS;
}

void my_init_cells(const int height, const int width, int cell[height][width], FILE* fp){
    if(fp==NULL){
        srand((unsigned int)time(NULL));
        for(int y = 0 ; y < height ; y++){
            for(int x = 0 ; x < width ; x++){
                int n=0;
                n=rand()%10;
                if(n==0){
                    cell[y][x]=1;
                }else{
                    cell[y][x]=0;
                }
            }
        }
    }else{
        int x[width];
        int y[height];
        int c=0;
        char s[100];
        fgets(s,100,fp);
        if(strcmp(s,"#Life 1.06\n")==0){
            while(fscanf(fp,"%d %d",&x[c],&y[c])!=EOF){
                c++;
            }
            for(int i=0;i<c;++i){
                cell[y[i]][x[i]]=1;
            }      
        }else{
            while(1){
                if(s[0]!='#'){
                    break;
                }
            }
            char s[1000];
            int i=0;
            while(1){
                s[i]=fgetc(fp);
                ++i;
                if(s[i]=='!'){
                    break;
                }
            }
            for(int j=0;j<i-1;++j){
                int f=0;
                if(s[j]=='b'){
                    if(j==0){
                        cell[0][0]=1;
                        f+=1;
                    }else if((s[j-1]=='o')||(s[j-1]=='$')){
                        cell[f/width][f-f*(f/width)]=1;
                        f+=1;
                    }else{
                        for(int k=0;k<s[j-1];++k){
                            cell[(f+k)/width][f+k-(f+k)*((f+k)/width)]=1;
                            f+=1;
                        }
                    }
                }else if(s[j]=='o'){
                    if(j==0){
                        cell[0][0]=0;
                        f+=1;
                    }else if((s[j-1]=='b')||(s[j-1]=='$')){
                        cell[f/width][f-f*(f/width)]=0;
                        f+=1;
                    }else{
                        for(int k=0;k<s[j-1];++k){
                            cell[(f+k)/width][f+k-(f+k)*((f+k)/width)]=0;
                            f+=1;
                        }
                    }
                }
            }   
        }         
    }
}

void my_print_cells(FILE* fp, int gen, const int height, const int width, int cell[height][width]){
    double sum=0;
    for(int y = 0 ; y < height ; y++){
        for(int x = 0 ; x < width ; x++){
            sum+=cell[y][x];
        }
    }
    FILE *fp2=stdout;    
    fprintf(fp2,"%f\n",sum/(height*width));

    fprintf(fp2,"+");
    for (int x = 0 ; x < width ; x++){
        fprintf(fp, "-");
    }
    fprintf(fp2, "+\n");

    for (int y = 0; y < height; y++) {
        fprintf(fp2,"|");
        for (int x = 0; x < width; x++) {
            if(cell[y][x]){
	            fprintf(fp2, "\e[31m#\e[0m");
            }else{
	            fprintf(fp2, " ");
            }
        }
        fprintf(fp2,"|\n");
    }

    fprintf(fp2, "+");
    for (int x = 0 ; x < width ; x++){
        fprintf(fp2, "-");
    }
    fprintf(fp2, "+\n");

    fflush(fp2);

}

void put_cells(int gen, const int height, const int width, int cell[height][width]){
    if((gen%100==0)&&(gen<10000)){
        char filename[100];
        sprintf(filename,"gen%04d.lif",gen);
        FILE *fp3=fopen(filename,"w");
        fprintf(fp3,"#Life 1.06\n");
        for(int y = 0 ; y < height ; y++){
            for(int x = 0 ; x < width ; x++){
                if(cell[y][x]==1){
                    fprintf(fp3,"%d %d\n",x,y);
                }
            }
        }
        fclose(fp3);
    }
}

int my_count_adjacent_cells(int h, int w, const int height, const int width, int cell[height][width]){
            if(h==0){
                if(w==0){
                    return cell[h][w+1]+cell[h+1][w]+cell[h+1][w+1];
                }else if(w==width-1){
                    return cell[h][w-1]+cell[h+1][w]+cell[h+1][w-1];
                }else{
                    return cell[h][w+1]+cell[h+1][w]+cell[h+1][w+1]+cell[h+1][w-1]+cell[h][w-1];
                }
            }else if(h==height-1){
                if(w==0){
                    return cell[h][w+1]+cell[h-1][w]+cell[h-1][w+1];
                }else if(w==width-1){
                    return cell[h][w-1]+cell[h-1][w]+cell[h-1][w-1];
                }else{
                    return cell[h][w+1]+cell[h-1][w]+cell[h-1][w+1]+cell[h-1][w-1]+cell[h][w-1];
                }
            }else{
                return (cell[h][w+1]+cell[h-1][w]+cell[h-1][w+1]+cell[h-1][w-1]+cell[h][w-1]+cell[h+1][w-1]+cell[h+1][w]+cell[h+1][w+1]);
            }
}

void my_update_cells(const int height, const int width, int cell[height][width]){
    for(int y = 0 ; y < height ; y++){
        for(int x = 0 ; x < width ; x++){
            if (cell[y][x]==1){
                if(!((my_count_adjacent_cells(y,x,height,width,cell)==2)||(my_count_adjacent_cells(y,x,height,width,cell)==3))){
                    cell[y][x]=0;
                }
            }else{
                if(my_count_adjacent_cells(y,x,height,width,cell)==3){
                    cell[y][x]=1;
                }
            }
        }
    }
}



            


