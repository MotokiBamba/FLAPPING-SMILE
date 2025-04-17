#include "gba1.h"

void draw_player(hword, int);
void draw_wall(hword, hword, hword);
void print_anycolor(byte *str, hword, hword);
void print_bgcolor(hword);
void display_time1(hword);
void display_time2(hword);
int collision_check(hword);
void locateplayer(hword, hword);
void locatewall(hword, hword);

extern unsigned char char8x8[256][8];
extern point p;

point playerp;
point wallp;
hword titleback = BGR(0x1F, 0x00, 0x00);    // タイトル画面の背景色
hword titleword = BGR(0x1F, 0x1F, 0x00);    // タイトル画面の文字の色
hword explainword = BGR(0x00, 0x00, 0x00);  // 説明文の色
hword gameback = BGR(0x00, 0x00, 0x1F);     // ゲーム画面の背景色
hword gameword = BGR(0x1F, 0x1F, 0x1F);     // ゲーム画面の文字の色
hword pauseword = BGR(0x1F, 0x00, 0x00);    // ポーズ画面の文字の色
hword resultback = BGR(0x1F, 0x1F, 0x1F);   // リザルト画面の背景色
hword resultword = BGR(0x00, 0x00, 0x00);   // リザルト画面の文字の色
hword playercolor = BGR(0x1F, 0x00, 0x00);  // プレイヤーの色
hword wallcolor = BGR(0x00, 0x1F, 0x00);    // 壁の色

byte wall0[] = "AAAAAAAA   AAAAAAAA";
byte wall1[] = "B   BBBBBBBBBBBBBBB";
byte wall2[] = "CCCCCCCCCCCCCC   CC";
byte wall3[] = "DDDDDDDDDD   DDDDDD";
byte wall4[] = "EEEEE  EEEEEEEEEEEE";
byte wall[21];

int main(void) {

    hword playerx;   // プレイヤーの初期x座標
    hword playery;   // プレイヤーの初期y座標
    hword wallx;     // 壁の初期x座標
    hword wally;     // 壁の初期y座標
    volatile unsigned short time;
    volatile unsigned short besttime = 0;
    int pausecheck = 0;
    int updatecheck = 0;
    int random;
    int i;

	/* 画面初期化ルーチン */
	*((unsigned short *)IOBASE) = 0xF03;

	/* タイマカウンタ設定レジスタ */
	*((unsigned short *)0x04000100) = 0xFFFF - 1678 + 1;	// タイマ0 カウンタ設定
	*((unsigned short *)0x04000104) = 0xFFFF - 100 + 1;	// タイマ1 カウンタ設定
	*((unsigned short *)0x04000108) = 0xFFFF - 10 + 1;	// タイマ2 カウンタ設定
	*((unsigned short *)0x0400010C) = 0x0000;	// タイマ3 カウンタ設定

	*((unsigned short *)0x04000102) = 0x0080;	// タイマ0 制御設定
	*((unsigned short *)0x04000106) = 0x0084;	// タイマ1 制御設定
	*((unsigned short *)0x0400010A) = 0x0084;	// タイマ2 制御設定
	
    do {
        // タイトル画面
        besttime = 0;   // ベストタイム初期化

        print_bgcolor(titleback);   // 背景を赤に
        locate(8, 8);
        print_anycolor("FLAPPING SMILE", titleword, titleback);

        locate(7, 16);
        print_anycolor("Avoid green text.", explainword, titleback);

        while(1) {
            locate(10, 11);
            print_anycolor("press to A", titleword, titleback);
            locate(10, 11);
            print_anycolor("press to A", BGR(0x1F, 0x1F, 0x1F), titleback);
            if((~(*(hword *) KEY_STATUS) & KEY_ALL) == KEY_A) {
                break;
            }
        }

        do {
            // ゲームスタート
            // プレイヤー・壁の座標初期化
            playerx = 15;
            playery = 10;
            wallx = 29;
            wally = 1;
            
            for(i = 0; i < 21; i++) {   // 壁の初期化
                wall[i] = wall0[i];
            }

            // タイマ３初期化
            *((unsigned short *)0x0400010E) = 0x0004;
            *((unsigned short *)0x0400010C) = 0x0000;
            
            *((unsigned short *)0x0400010E) = 0x0084;	// 計測スタート（タイマ3 制御設定）

            print_bgcolor(gameback);   // 背景を青に

            while (1) {
                for(i = 0; i < 100000; i++);	

                locate(16, 0);
                print_anycolor("A:jump B:pause", gameword, gameback);

                // タイマ
                time = 	*((unsigned short *)0x0400010C);    // 計測値読み込み
                random = mod(*((unsigned short *)0x04000100), 5);  // 疑似乱数生成（0～4）
                
                // ポーズ
                if(pausecheck == 1) {   // ポーズが連続することを防止する
                    *((unsigned short *)0x0400010E) = 0x0004;   // 計測ストップ
                    *((unsigned short *)0x0400010C) = time;

                    while((~(*(hword *) KEY_STATUS) & KEY_ALL) == KEY_B);
                    
                    pausecheck = 0;

                    *((unsigned short *)0x0400010E) = 0x0084;   // 計測再開
                }

                if((~(*(hword *) KEY_STATUS) & KEY_ALL) == KEY_B) {
                    *((unsigned short *)0x0400010E) = 0x0004;   // 計測ストップ
                    *((unsigned short *)0x0400010C) = time;
                    
                    pausecheck = 1;

                    locate(12, 5);
                    print_anycolor("pause", pauseword, gameback);

                    locate(11, 9);
                    print_anycolor("B:resume", pauseword, gameback);

                    while((~(*(hword *) KEY_STATUS) & KEY_ALL) == KEY_B);
                    while((~(*(hword *) KEY_STATUS) & KEY_ALL) != KEY_B);

                    locate(12, 5);
                    print_anycolor("     ", gameword, gameback);

                    locate(11, 9);
                    print_anycolor("        ", gameword, gameback);

                    *((unsigned short *)0x0400010E) = 0x0084;   // 計測再開
                }

                // プレイヤー
                if((~(*(hword *) KEY_STATUS) & KEY_ALL) == KEY_A) { // Aボタンが押されたとき
                    if(playery > 1) {
                        draw_player(playery--, 32);
                        if(collision_check(playery) == 1) { // 当たり判定
                            break;
                        } else {
                            draw_player(playery, 2);
                        }
                    } else {
                        if(collision_check(playery) == 1) { // 当たり判定
                            break;
                        }
                    }
                } else if(mod(time, 2) == 0) { 
                    if(playery < 19) {
                        draw_player(playery++, 32);
                        if(collision_check(playery) == 1) { // 当たり判定
                            break;
                        } else {
                            draw_player(playery, 2);
                        }
                    } else {
                        if(collision_check(playery) == 1) { // 当たり判定
                            break;
                        } 
                    }
                } else {
                    if(collision_check(playery) == 1) { // 当たり判定
                        break;
                    }
                    draw_player(playery, 2);
                }

                // 壁
                // 壁の移動
                if(mod(time, 1) == 0) {
                    draw_wall(wallx--, wally, gameback);
                    if(wallx <= 0) {
                        wallx = 29;
                    }
                    draw_wall(wallx, wally, wallcolor);
                } else {
                    draw_wall(wallx, wally, wallcolor);
                }

                // 壁の選択
                if(wallx == 1) {
                    draw_wall(wallx, wally, gameback);
                    if(random == 0) {
                        for(i = 0; i < 21; i++) {
                            wall[i] = wall0[i];
                        }
                    } else if(random == 1) {
                        for(i = 0; i < 21; i++) {
                            wall[i] = wall1[i];
                        }
                    } else if(random == 2) {
                        for(i = 0; i < 21; i++) {
                            wall[i] = wall2[i];
                        }
                    } else if(random == 3) {
                        for(i = 0; i < 21; i++) {
                            wall[i] = wall3[i];
                        }
                    } else if(random == 4) {
                        for(i = 0; i < 21; i++) {
                            wall[i] = wall4[i];
                        }
                    }
                }

                locate(0,0);
                display_time1(time);
            }

            // リザルト
            print_bgcolor(resultback);    // 背景を白に

            locate(11, 4);
            print_anycolor("game over", resultword, resultback);

            locate(10, 7);
            print_anycolor("time:", resultword, resultback);
            locate(15, 7);
            display_time2(time);

            locate(5, 9);
            print_anycolor("your best:", resultword, resultback);
            if(time > besttime) {
                besttime = time;
                updatecheck = 1;
            }
            locate(15, 9);
            display_time2(besttime);
            if(updatecheck == 1) {
                locate(23, 9);
                print_anycolor("new", BGR(0x1F, 0x00, 0x00), resultback);
                updatecheck = 0;
            }

            locate(8, 12);
            print_anycolor("R:retry  L:title", resultword, resultback);

            while(1) {  // do-while文を抜けないようにするための無限ループ
                if((~(*(hword *) KEY_STATUS) & KEY_ALL) == KEY_R) { // Rボタンが押されたらbreak
                    break;
                }
                if((~(*(hword *) KEY_STATUS) & KEY_ALL) == KEY_L) { // Lボタンが押されたらbreak
                    break;
                }
            }
        } while((~(*(hword *) KEY_STATUS) & KEY_ALL) == KEY_R); // ゲーム画面へ
    } while((~(*(hword *) KEY_STATUS) & KEY_ALL) == KEY_L); // タイトル画面へ
		
	return 0;

}

void draw_player(hword py, int index) {   // プレイヤーの表示
    hword tx, ty;
    unsigned char cbit;

    locateplayer(15, py);

    for (ty = 0; ty < 8; ty++) {
        cbit = 0x80;						
                                            
        for (tx = 0; tx < 8; tx++) {
            if ((char8x8[index][ty] & cbit) == cbit) {
                draw_point((playerp.x + tx), (playerp.y + ty), playercolor);
            } else {
                draw_point((playerp.x + tx), (playerp.y + ty), gameback);
            }
            cbit = cbit >> 1;
        }
    }
}

void draw_wall(hword wx, hword wy, hword c) {    // 壁の表示
    hword tx, ty;
    unsigned char cbit;
    int index = 0;
    locatewall(wx, wy);

    while(wall[index]) {

        for (ty = 0; ty < 8; ty++) {
            cbit = 0x80;						
                                                
            for (tx = 0; tx < 8; tx++) {
                if ((char8x8[wall[index]][ty] & cbit) == cbit) {
                    draw_point((wallp.x + tx), (wallp.y + ty), c);
                }
                cbit = cbit >> 1;
            }
        }

        index++;
        wallp.y = wallp.y + CHAR_WIDTH;
    }
}

void print_anycolor(byte *str, hword c1, hword c2) {    // 指定した色の文字を表示
	
	hword tx, ty;							
	byte cbit;						

    while(*str) {

        for(ty = 0; ty < 8; ty++) {				
	
            cbit = 0x80;						
            
            for(tx = 0; tx < 8; tx++) {							
                if((char8x8[*str][ty] & cbit) == cbit){			
                    draw_point((p.x + tx), (p.y + ty), c1);	
                }else {
                    draw_point((p.x + tx), (p.y + ty), c2);	
                }
                cbit = cbit >> 1;					
            }
	    }

        str++;
        p.x = p.x + CHAR_WIDTH;
    }
}

void print_bgcolor(hword c) {   // 背景色の指定
    hword *ptr;
    int i;

    ptr = (hword *)VRAM;
    for(i = 0; i < 38400; i++) {
        *ptr++ = c;
    }
} 

void display_time1(hword val) {     // 時間の表示（m秒）

	byte char_data[] = "0123456789";
	byte buf[6];
	hword tmp;
	int i;
	
	/* 入力値valの桁数設定 */
	i = 3;

	/* 文字列の最後にNULLコード挿入 */
	buf[i+1] = 0;
	
	/* 最下位桁の文字（10で割った余り）を挿入し，入力値を10で割る */
	for(; i >= 0; i--) {
		buf[i] = char_data[mod(val, 10)];
		val = div(val, 10);
	}
	
	/* 文字列全体を表示 */
	print_anycolor(buf, gameword, gameback);
	
	return;
}

void display_time2(hword val) {     // 時間の表示（分:秒.m秒）

	byte char_data[] = "0123456789";
	byte buf[8];
	hword tmp;
    hword m;    // 分
    hword s;    // 秒
    hword ms;   // m秒
	int mindex;
    int sindex;

    m = div(div(val, 10), 60);
    s = mod(div(val, 10), 60);
    ms = mod(val, 10);
	
	mindex = 1;
	for(; mindex >= 0; mindex--) {  // 分部分の格納
		buf[mindex] = char_data[mod(m, 10)];
		m = div(m, 10);
	}

    buf[2] = ':';

    sindex = 4;
    for(; sindex >= 3; sindex--) {  // 秒部分の格納
		buf[sindex] = char_data[mod(s, 10)];
		s = div(s, 10);
	}

    buf[5] = '.';
    buf[6] = char_data[mod(ms, 10)];
    buf[7] = 0;
	
	/* 文字列全体を表示 */
	print_anycolor(buf, resultword, resultback);
	
	return;
}

int collision_check(hword playery) {    // プレイヤーが壁に当たったかどうかチェック
    hword *ptr;
    int check;
    int i;

    locateplayer(15, playery);
    ptr = (hword *) VRAM + (playerp.x + 3) + playerp.y * LCD_WIDTH;

    if(*ptr == wallcolor) {
        check = 1;
    } else {
        check = 0;
    }

    return check;
}

void locateplayer(hword cx, hword cy) {
    playerp.x = cx * 8;
    playerp.y = cy * 8;
}

void locatewall(hword cx, hword cy) {
    wallp.x = cx * 8;
    wallp.y = cy * 8;
}