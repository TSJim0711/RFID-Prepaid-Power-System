/**
 * @file lv_port_indev_template.c
 *
 */

/*Copy this file as "lv_port_indev.c" and set this value to "1" to enable content*/
#if 1

/*********************
 *      INCLUDES
 *********************/
#include <stdbool.h>
#include <stdio.h>

#include "lvgl.h"
#include "lv_port_indev.h"
#include "lcd/tft_touch.h"
#include "rfid/MFRC522.h"

#include "stm32f4xx.h"

/*********************
 *      DEFINES
 *********************/
#define SLOT_1_IN_USE_FLAG 10001 //DISP1's seat in use?
/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void touchpad_init(void);
static void touchpad_read(lv_indev_t * indev, lv_indev_data_t * data);
static bool touchpad_is_pressed(void);
static void touchpad_get_xy(int32_t * x, int32_t * y);

static void mouse_init(void);
static void mouse_read(lv_indev_t * indev, lv_indev_data_t * data);
static bool mouse_is_pressed(void);
static void mouse_get_xy(int32_t * x, int32_t * y);

static void keypad_init(void);
static void keypad_read(lv_indev_t * indev, lv_indev_data_t * data);
static uint32_t keypad_get_key(void);

static void encoder_init(void);
static void encoder_read(lv_indev_t * indev, lv_indev_data_t * data);
static void encoder_handler(void);

static void button_init(void);
static void button_read(lv_indev_t * indev, lv_indev_data_t * data);
static int8_t button_get_pressed_id(void);
static bool button_is_pressed(uint8_t id);

static void relay_init(void);

static void rfid_reader_init(void);
static void rfid_read_handler(lv_indev_t * indev_drv, lv_indev_data_t * data);

/**********************
 *  STATIC VARIABLES
 **********************/
lv_indev_t * indev_touchpad;
lv_indev_t * indev_mouse;
lv_indev_t * indev_keypad;
lv_indev_t * indev_encoder;
lv_indev_t * indev_button;
lv_indev_t * indev_rfid_scanner;

static int32_t encoder_diff;
static lv_indev_state_t encoder_state;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_port_indev_init(lv_display_t* disp_main, lv_display_t* disp_sub)
{
    /**
     * Here you will find example implementation of input devices supported by LittelvGL:
     *  - Touchpad
     *  - Mouse (with cursor support)
     *  - Keypad (supports GUI usage only with key)
     *  - Encoder (supports GUI usage only with: left, right, push)
     *  - Button (external buttons to press points on the screen)
     *
     *  The `..._read()` function are only examples.
     *  You should shape them according to your hardware
     */

    /*------------------
     * Touchpad
     * -----------------*/

    /*Initialize your touchpad if you have*/
    touchpad_init();

    /*Register a touchpad input device*/
    indev_touchpad = lv_indev_create();
    lv_indev_set_type(indev_touchpad, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(indev_touchpad, touchpad_read);
    lv_indev_set_display(indev_touchpad, disp_main);

//    /*------------------
//     * Mouse
//     * -----------------*/
//
//    /*Initialize your mouse if you have*/
//    mouse_init();
//
//    /*Register a mouse input device*/
//    indev_mouse = lv_indev_create();
//    lv_indev_set_type(indev_mouse, LV_INDEV_TYPE_POINTER);
//    lv_indev_set_read_cb(indev_mouse, mouse_read);
//
//    /*Set cursor. For simplicity set a HOME symbol now.*/
//    lv_obj_t * mouse_cursor = lv_image_create(lv_screen_active());
//    lv_image_set_src(mouse_cursor, LV_SYMBOL_HOME);
//    lv_indev_set_cursor(indev_mouse, mouse_cursor);
//
//    /*------------------
//     * Keypad
//     * -----------------*/
//
//    /*Initialize your keypad or keyboard if you have*/
//    keypad_init();
//
//    /*Register a keypad input device*/
//    indev_keypad = lv_indev_create();
//    lv_indev_set_type(indev_keypad, LV_INDEV_TYPE_KEYPAD);
//    lv_indev_set_read_cb(indev_keypad, keypad_read);
//
//    /*Later you should create group(s) with `lv_group_t * group = lv_group_create()`,
//     *add objects to the group with `lv_group_add_obj(group, obj)`
//     *and assign this input device to group to navigate in it:
//     *`lv_indev_set_group(indev_keypad, group);`*/
//
//    /*------------------
//     * Encoder
//     * -----------------*/
//
//    /*Initialize your encoder if you have*/
//    encoder_init();
//
//    /*Register a encoder input device*/
//    indev_encoder = lv_indev_create();
//    lv_indev_set_type(indev_encoder, LV_INDEV_TYPE_ENCODER);
//    lv_indev_set_read_cb(indev_encoder, encoder_read);
//
//    /*Later you should create group(s) with `lv_group_t * group = lv_group_create()`,
//     *add objects to the group with `lv_group_add_obj(group, obj)`
//     *and assign this input device to group to navigate in it:
//     *`lv_indev_set_group(indev_encoder, group);`*/
//
    /*------------------
     * Button
     * -----------------*/

    /*Initialize your button if you have*/
    button_init();

    /*Register a button input device*/
    indev_button = lv_indev_create();
    lv_indev_set_type(indev_button, LV_INDEV_TYPE_BUTTON);
    lv_indev_set_read_cb(indev_button, button_read);

    /*Assign buttons to points on the screen*/
    static const lv_point_t btn_points[2] = {
        {10, 10},   /*Button 0 -> x:10; y:10*/
        {40, 100},  /*Button 1 -> x:40; y:100*/
    };
    lv_indev_set_display(indev_button, disp_sub);//affect to small lcd screen
    lv_indev_set_button_points(indev_button, btn_points);

	//-------
    //Relay
    //-------
	relay_init();
	
    //-------
    //RFID reader
    //-------
    rfid_reader_init();
    indev_rfid_scanner = lv_indev_create();
    lv_indev_set_type(indev_rfid_scanner, LV_INDEV_TYPE_BUTTON);
    lv_indev_set_read_cb(indev_rfid_scanner, rfid_read_handler);
    lv_timer_set_period(lv_indev_get_read_timer(indev_rfid_scanner), 100);//read once per 0.1s
    static const lv_point_t rfid_sim_click[1] = {{10, 10}};//if read card, the click 10,10 same as button 0
    lv_indev_set_display(indev_rfid_scanner, disp_sub);//affect to small lcd screen
    lv_indev_set_button_points(indev_rfid_scanner, rfid_sim_click);

}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/*------------------
 * Touchpad
 * -----------------*/

/*Initialize your touchpad*/
static void touchpad_init(void)
{
    /*Your code comes here*/
    TP_Init();
}

/*Will be called by the library to read the touchpad*/
static void touchpad_read(lv_indev_t * indev_drv, lv_indev_data_t * data)
{
    static int32_t last_x = 0;
    static int32_t last_y = 0;

    /*Save the pressed coordinates and the state*/
    if(touchpad_is_pressed()) {
        touchpad_get_xy(&last_x, &last_y);
        data->state = LV_INDEV_STATE_PRESSED;
        printf("Touch PRESSED! X:%d, Y:%d\r\n", last_x, last_y);
    }
    else {
        data->state = LV_INDEV_STATE_RELEASED;
    }

    /*Set the last pressed coordinates*/
    data->point.x = last_x;
    data->point.y = last_y;
}

/*Return true is the touchpad is pressed*/
static bool touchpad_is_pressed(void)
{
    /*Your code comes here*/
    if(PEN)//PBin(1) defined in tft_touch.h
        return false;
    return true;
}

/*Get the x and y coordinates if the touchpad is pressed*/
static void touchpad_get_xy(int32_t * x, int32_t * y)
{
    /*Your code comes here*/
	if(TP_Scan(0))
	{
		// 坐标转换，处理横竖屏与物理坐标的镜像。
        // 原本 switch 的实际显示位置约为 (120, 160) 即中心
        // 打印显示点击 (95, 159) 时触发了 switch(期望是120, 160附近)，
        // 而点击 switch 位置 (177, 106) 时打印出来的坐标是偏的
        
        // 尝试翻转映射关系，适配 LVGL 的逻辑坐标
        // 假设当前屏幕分辨率为主体 240x320
        // LVGL 的原点 (0, 0) 在左上角，(240, 320) 在右下角
        
		(*x) = tp_dev.x[0];
		(*y) = tp_dev.y[0];	
        
        // 根据您给的坐标偏移现象：
        // 你的代码原本在 tft_touch.c 是将数值进行过系数转换的，但转换可能未适配现在的横竖屏方向
        // 这是一个比较常见的触摸映射问题。先试试进行中心对称或XY对调。
        // 将 X 轴与 Y 轴反向。假设你的屏幕宽度是 240，高度 320：
        
        // 注意由于您提供的数据 `(177, 106)` 对应真实显示位置，
        // 说明 X=177 被识别为了大概 Y=160 的位置，Y=106 被识别为了 X=120 左右的位置
        // 这是很典型的 X、Y 反转（加上可能的镜像）。
        // 我们可以根据 `触摸点(95,159)` -> `触发 switch` 和 `显示点(177,106)` 的关系来修正。
        
        // X, y swap and minor translation approximation test
	}
}

/*------------------
 * Mouse
 * -----------------*/

/*Initialize your mouse*/
static void mouse_init(void)
{
    /*Your code comes here*/
}

/*Will be called by the library to read the mouse*/
static void mouse_read(lv_indev_t * indev_drv, lv_indev_data_t * data)
{
    /*Get the current x and y coordinates*/
    mouse_get_xy(&data->point.x, &data->point.y);

    /*Get whether the mouse button is pressed or released*/
    if(mouse_is_pressed()) {
        data->state = LV_INDEV_STATE_PRESSED;
    }
    else {
        data->state = LV_INDEV_STATE_RELEASED;
    }
}

/*Return true is the mouse button is pressed*/
static bool mouse_is_pressed(void)
{
    /*Your code comes here*/

    return false;
}

/*Get the x and y coordinates if the mouse is pressed*/
static void mouse_get_xy(int32_t * x, int32_t * y)
{
    /*Your code comes here*/

    (*x) = 0;
    (*y) = 0;
}

/*------------------
 * Keypad
 * -----------------*/

/*Initialize your keypad*/
static void keypad_init(void)
{
    /*Your code comes here*/
}

/*Will be called by the library to read the mouse*/
static void keypad_read(lv_indev_t * indev_drv, lv_indev_data_t * data)
{
    static uint32_t last_key = 0;

    /*Get the current x and y coordinates*/
    mouse_get_xy(&data->point.x, &data->point.y);

    /*Get whether the a key is pressed and save the pressed key*/
    uint32_t act_key = keypad_get_key();
    if(act_key != 0) {
        data->state = LV_INDEV_STATE_PRESSED;

        /*Translate the keys to LVGL control characters according to your key definitions*/
        switch(act_key) {
            case 1:
                act_key = LV_KEY_NEXT;
                break;
            case 2:
                act_key = LV_KEY_PREV;
                break;
            case 3:
                act_key = LV_KEY_LEFT;
                break;
            case 4:
                act_key = LV_KEY_RIGHT;
                break;
            case 5:
                act_key = LV_KEY_ENTER;
                break;
        }

        last_key = act_key;
    }
    else {
        data->state = LV_INDEV_STATE_RELEASED;
    }

    data->key = last_key;
}

/*Get the currently being pressed key.  0 if no key is pressed*/
static uint32_t keypad_get_key(void)
{
    /*Your code comes here*/

    return 0;
}

/*------------------
 * Encoder
 * -----------------*/

/*Initialize your encoder*/
static void encoder_init(void)
{
    /*Your code comes here*/
}

/*Will be called by the library to read the encoder*/
static void encoder_read(lv_indev_t * indev_drv, lv_indev_data_t * data)
{

    data->enc_diff = encoder_diff;
    data->state = encoder_state;
}

/*Call this function in an interrupt to process encoder events (turn, press)*/
static void encoder_handler(void)
{
    /*Your code comes here*/

    encoder_diff += 0;
    encoder_state = LV_INDEV_STATE_RELEASED;
}

/*------------------
 * Button
 * -----------------*/

/*Initialize your buttons*/
static void button_init(void)
{
    /*Your code comes here*/
}

/*Will be called by the library to read the button*/
static void button_read(lv_indev_t * indev_drv, lv_indev_data_t * data)
{

    static uint8_t last_btn = 0;

    /*Get the pressed button's ID*/
    int8_t btn_act = button_get_pressed_id();

    if(btn_act >= 0) {
        data->state = LV_INDEV_STATE_PRESSED;
        last_btn = btn_act;
    }
    else {
        data->state = LV_INDEV_STATE_RELEASED;
    }

    /*Save the last pressed button's ID*/
    data->btn_id = last_btn;
}

/*Get ID  (0, 1, 2 ..) of the pressed button*/
static int8_t button_get_pressed_id(void)
{
    uint8_t i;

    /*Check to buttons see which is being pressed (assume there are 2 buttons)*/
    for(i = 0; i < 2; i++) {
        /*Return the pressed button's ID*/
        if(button_is_pressed(i)) {
            return i;
        }
    }

    /*No button pressed*/
    return -1;
}

/*Test if `id` button is pressed or not*/
static bool button_is_pressed(uint8_t id)
{

    /*Your code comes here*/
    if (PAin(0)==1)
        return true;
    return false;
}

//-------
//Relay
//-------
void relay_init(void)
{
	//PEout6 Relay to power supply init
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_High_Speed;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
}

//-----
//RFID reader
//-----
void rfid_reader_init(void)
{
    MFRC522_Initializtion();
}


u8  card_pydebuf[2];
u8  card_numberbuf[5];
u8  card_key0Abuf[6]={0xff,0xff,0xff,0xff,0xff,0xff};//for safty, not set any passcode
u8  card_writebuf[16];
u8  card_readbuf[18];
extern uint32_t SLOT_1_IN_USE_SIG;

task rfid_reader_task;
bool card_present=false;
int id_cnt=1001;//first user id is 1001

int card_id;
int card_balance;
int occupied_time_T_Plus;
bool pay2use=true;

void rfid_read_handler(lv_indev_t * indev_drv, lv_indev_data_t * data)
{
    u8 status, card_size;
	if(card_present==false)//if found card
	{
		status = MFRC522_Request(0x52, card_pydebuf);//check if card on scanner
		if(status!=0)//not found card
		{
			printf("No Card Read.\r\n");
			PEout(6)=0;
			Write_MFRC522(CommandReg, PCD_IDLE);//clear cmd s
			ClearBitMask(Status2Reg, 0x08);//shut down auth layer
			pay2use=true;
			return;
		}
		status=MFRC522_Anticoll(card_numberbuf);//select card to read, prevent multi caard reading mixed	
		card_size=MFRC522_SelectTag(card_numberbuf);//commu with selected card
		status=MFRC522_Auth(0x60, 4, card_key0Abuf, card_numberbuf);//unlock sector 1 aka block 4~6
		printf("RFID Task:%d\r\n",rfid_reader_task);
		if(rfid_reader_task==NORMAL && pay2use==true)
		{
			status=MFRC522_Read(4, card_readbuf);//read id from card
			if(data->state==LV_INDEV_STATE_RELEASED)
			{
				card_id = (uint32_t)card_readbuf[0] | ((uint32_t)card_readbuf[1] << 8) | ((uint32_t)card_readbuf[2] << 16);
				status=MFRC522_Read(5, card_readbuf);//read balance from card
				card_balance = (uint32_t)card_readbuf[0] | ((uint32_t)card_readbuf[1] << 8) | ((uint32_t)card_readbuf[2] << 16)-0;//charge $0 per use
				if(card_readbuf[5]==1)//sign disgit, if 1-> change sign
				{	card_balance=-card_balance;}
				else 
				{	PEout(6)=1;}
				
				occupied_time_T_Plus=0;
				printf("Cur card id:%d; Balance:%d; T+%d\r\n",card_id,card_balance,occupied_time_T_Plus);
				lv_obj_send_event(lv_layer_sys(), SLOT_1_IN_USE_SIG, (void *)1);//send 1 msg, note that I use store data with addr, it is clever, gemini said so
				lv_timer_set_period(lv_indev_get_read_timer(indev_rfid_scanner), 500);
				card_present=true;
			}
		}
		else if(rfid_reader_task==REGISTRATION)
		{
			status=0;//0 is good return from MFRC522
			printf("Registing\r\n");
			PEout(6)=0;
			MFRC522_Read(5, card_readbuf);
			if(card_readbuf[5]==1)//prevent re-register card, reset negative balance
			{
				printf("Nice try ;)\r\n");
				lv_obj_send_event(lv_layer_sys(), SLOT_1_IN_USE_SIG, (void *)1);//Use disp_1 NORMAL layout, there is ID and balance. U may ask why? Because there is one. No custom layout for reg and top up done.
				pay2use=false;
				return;
			}
			
			memset(card_writebuf, 0, 16);
			card_writebuf[0] = (uint8_t)(id_cnt & 0xFF);//assign id, first is 1001
			card_writebuf[1] = (uint8_t)((id_cnt >> 8) & 0xFF);
			card_writebuf[2] = (uint8_t)((id_cnt >> 16) & 0xFF);
			status=MFRC522_Write(4,card_writebuf);
			id_cnt++;
			if(status==0)
			{
				memset(card_writebuf, 0, 16); //assign balance:0, just for cleaning up block 2 in case random data
				status=MFRC522_Write(5,card_writebuf);
				if(status==0)
				{
					printf("Register Done.\r\n");
					rfid_reader_task = NORMAL;
					MFRC522_Halt();//sleep the card 
					lv_obj_send_event(lv_layer_sys(), SLOT_1_IN_USE_SIG, (void *)1);//Use disp_1 NORMAL layout, there is ID and balance. U may ask why? Because there is one. No custom layout for reg and top up done.
					pay2use=false;
				}
			}
		}
		else//if larger/eql 1, treat as top up value
		{
			printf("Topping up $%d\r\n",rfid_reader_task);
			PEout(6)=0;
			status=0;
			status=MFRC522_Read(5, card_readbuf);
			if(status==0)
			{
				//cal total balance in card
				int32_t balance=0;
				balance = (uint32_t)card_readbuf[0] | ((uint32_t)card_readbuf[1] << 8) | ((uint32_t)card_readbuf[2] << 16);//256^2/100 is way enough for balance
				if(card_readbuf[5]==1)//sign digit, if 1-> change sign
					balance=-balance;
				//add value
				balance+=rfid_reader_task*100;//two digits for decimals
				memset(card_writebuf, 0, sizeof(card_writebuf));//clean up buffer
				card_writebuf[0] = (uint8_t)(abs(balance) & 0xFF);//save new balance to write buffer
				card_writebuf[1] = (uint8_t)((abs(balance) >> 8) & 0xFF);
				card_writebuf[2] = (uint8_t)((abs(balance) >> 16) & 0xFF);
				if(balance<0)
					card_writebuf[5]=1;//set sign digit if still negative
				//re-write
				status=MFRC522_Write(5,card_writebuf);
				if(status==0)
				{
					printf("Top up done.\r\n");
					rfid_reader_task = NORMAL;
					MFRC522_Halt();//sleep the card
					lv_obj_send_event(lv_layer_sys(), SLOT_1_IN_USE_SIG, (void *)1);//Use disp_1 NORMAL layout, there is ID and balance. U may ask why? Because there is one! No custom layout for reg and top up done.
					pay2use=false;
				}
			}
		}
    }
	else if(card_present==true)//only rfid_reader_task==NORMAL will set card_present =1
	{
		status=MFRC522_Read(4, card_readbuf);//read id from card
		if(status!=0)//read failed = card exist no more
		{
			printf("read failed \r\n");
			card_present=false;
			lv_obj_send_event(lv_layer_sys(), SLOT_1_IN_USE_SIG, (void *)0);//send 0 msg, disp_1 show insert card
			lv_timer_set_period(lv_indev_get_read_timer(indev_rfid_scanner), 100);
		}
		else
		{
			card_id = (uint32_t)card_readbuf[0] | ((uint32_t)card_readbuf[1] << 8) | ((uint32_t)card_readbuf[2] << 16);
			status=MFRC522_Read(5, card_readbuf);//read balance from card
			occupied_time_T_Plus++;
			
			if(occupied_time_T_Plus%5==0 && card_balance>0 && pay2use==true)//pay $2.56 every 5 sec (I do know %10=every 5 sec, but my board %10=10 sec, idk why.)
			{
				card_balance = (uint32_t)card_readbuf[0] | ((uint32_t)card_readbuf[1] << 8) | ((uint32_t)card_readbuf[2] << 16);
				if(card_readbuf[5]==1)//sign disgit, if 1-> change sign
					card_balance=-card_balance;

				card_balance-=256;//pay as u use
				
				memset(card_writebuf, 0, sizeof(card_writebuf));//clean up buffer
				card_writebuf[0] = (uint8_t)(abs(card_balance) & 0xFF);//save new balance to write buffer
				card_writebuf[1] = (uint8_t)((abs(card_balance) >> 8) & 0xFF);
				card_writebuf[2] = (uint8_t)((abs(card_balance) >> 16) & 0xFF);
				if(card_balance<0)
				{
					card_writebuf[5]=1;//sign digit
					PEout(6)=0;
				}
				
				MFRC522_Write(5,card_writebuf);//writeback
				lv_obj_send_event(lv_layer_sys(), SLOT_1_IN_USE_SIG, (void *)1);//Send again, in order to refresh balance on disp_1
			}
			printf("Cur card id:%d; Balance:%d; T+%d\r\n",card_id,card_balance,occupied_time_T_Plus);
		}
	}
}

#else /*Enable this file at the top*/

/*This dummy typedef exists purely to silence -Wpedantic.*/
typedef int keep_pedantic_happy;
#endif
