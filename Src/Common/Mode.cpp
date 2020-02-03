/**
 * Mode.cpp
 * @author yuta seya
 * @date 2019 3.28 
*/

#include "Mode.h"

#include <iostream>

#include <unistd.h>

#include <cstdio>

#include "TargetGenerator.h"

#include <mutex> 
extern std::mutex mtx;

/**
 * @brief モードクラスのコンストラクタ
 * @param なし
 * @return　なし
*/
Mode::Mode()
{

}

/**
 * @brief モードクラスのデストラクタ
 * @param なし
 * @return　なし
*/
Mode::~Mode()
{

}

/**
 * @brief モードクラスの初期化
 * @param なし
 * @return　なし
*/
void Mode::init()
{
  if ( led == nullptr ){
    led = Led::getInstance();
  }

  if ( sw == nullptr ){
    sw = Switch::getInstance();
  }

  if ( sensor == nullptr ){
    sensor = Sensor::getInstance();
  }

  if ( motor == nullptr ){
    motor = Motor::getInstance();
  }

  if ( maze == nullptr ){
    maze = Maze::getInstance();
  } 

  if ( trape == nullptr ){
    trape = Trapezoid::getInstance();
  } 

  if ( interrupt == nullptr ){
    interrupt = Interrupt::getInstance();
  } 
  
}

/**
 * @brief モードセレクト
 * @param なし
 * @return　なし
*/
void Mode::select()
{
  init();
  gx = 7;
  gy = 7;

  sensor->setConstant(1000, 120, Front);
  sensor->setConstant(495, 140, Left);
  sensor->setConstant(320, 120, Right);

  bool sw0,sw1,sw2;
  int mode_count = 0;

  while( 1 ){
    while( 1 ){
      mtx.lock();
      sw0 = sw->get0();
      sw1 = sw->get1();
      sw2 = sw->get2();
      std::printf("sw0 : %d, sw1 : %d, sw2 : %d\r\n",sw0, sw1, sw2 );
      mtx.unlock();

      if ( sw0 ){
        mode_count++;
        if ( mode_count > 8 ) mode_count = 0;
        mtx.lock();
        led->illuminate( mode_count );
        mtx.unlock();
      }

      if ( sw1 ){
        mode_count--;
        if ( mode_count < 0 ) mode_count = 8;
        mtx.lock();
        led->illuminate( mode_count );
        mtx.unlock();
      }

      if ( sw2 ) {
        mtx.lock();
        led->illuminate( 0x00 );
        mtx.unlock();
        break;
      }
      usleep(100);
    }

    if( mode_count == 0){
      Position pos;
      pos.init();
      mtx.lock();
      maze->show( pos );
      mtx.unlock();
    } else if( mode_count == 1){
      while( 1 ){
        mtx.lock();
        sensor->show();
        sw1 = sw->get1();
        mtx.unlock();
        if ( sw1 ) break;
        usleep(10000);
      }
    } else if( mode_count == 2 ){
      float vel = 0.0f;

      trape->makeTrapezoid( 180.0f, 2000.0f, 500.0f, 0.0f, 0.0f, false );

      while( trape->status() == false ){
        vel = trape->getNextVelocity();
        mtx.lock();
        std::printf( "%f\n", vel );
        mtx.unlock();
      }
    } else if( mode_count == 3 ){
      float velocity = 0.0f;
      int left = 0;
      int right = 0;

      TargetGenerator *trans_target = new TargetGenerator();

      trape->makeTrapezoid( 180.0f, 2000.0f, 500.0f, 0.0f, 0.0f, false );
      mtx.lock();
      std::printf("velocity ,left, right \r\n");
      mtx.unlock();

      while( trape->status() == false ){
        velocity = trape->getNextVelocity();
        trans_target->calcStepFrequency( velocity );
        trans_target->getStepFrequency( &left, &right, false );
        mtx.lock();
        std::printf("%5.5f, %d, %d\r\n", velocity, left, right );
        mtx.unlock();
        left = 0;
        right = 0;
      }
      delete trans_target;
    } else if( mode_count == 4 ){
      trape->makeTrapezoid( TURN_90, 1000.0f, 200.0f, 0.0f, 0.0f, true );
      while( trape->status() == false );

      usleep( 300 );
        
      trape->makeTrapezoid( -TURN_90, 1000.0f, 200.0f, 0.0f, 0.0f, true );
      while( trape->status() == false );
    } else if( mode_count == 5 ){
      Position pos;
      ExistWall exist;
      uint8_t goal_x = gx;
      uint8_t goal_y = gy;
      maze->setGoal(goal_x, goal_y);
      pos.init();
	    maze->resetMap();
      mtx.lock();
      led->illuminate(0x0f);
      mtx.unlock();
      sleep(1);
      mtx.lock();
      led->illuminate(0x01);
      mtx.unlock();
      sleep(1);
      uint8_t next_dir = Front;

      maze->updatePosition(&pos, next_dir);
      trape->makeTrapezoid( 90.0f, 2000.0f, 300.0f, 0.0f, 0.0f, false );
      while( trape->status() == false );
        
      while(pos.x != goal_x || pos.y != goal_y){	
        usleep(5500);
        mtx.lock();
        sw0 = sw->get0();
        mtx.unlock();

        for(int i = 0; i < 100; i++ ){
          mtx.lock();
          sensor->update();
          sensor->getWalldata(&exist);
          mtx.unlock();
          usleep(1);
        }

        if( sw0 ) break;
        next_dir = maze->getNextAction(&pos, &exist);
        //mtx.lock();
        //std::printf("%d, %d, %d, %d\r\n", exist.front, exist.left, exist.right, next_dir); 
        //mtx.unlock();
          
        if( next_dir == Front){
          maze->updatePosition(&pos, next_dir);
          trape->makeTrapezoid( 180.0f, 2000.0f, 300.0f, 0.0f, 0.0f, false );
          interrupt->setSideSensorControl(true);
          while( trape->status() == false );
          interrupt->setSideSensorControl(false);
        } else if( next_dir == Left){
          maze->updatePosition(&pos, next_dir);
          trape->makeTrapezoid( 90.0f, 2000.0f, 300.0f, 0.0f, 0.0f, false );
          interrupt->setSideSensorControl(true);
          while( trape->status() == false );
          interrupt->setSideSensorControl(false);
          trape->makeTrapezoid( TURN_90, 1000.0f, 200.0f, 0.0f, 0.0f, true );
          while( trape->status() == false );
          trape->makeTrapezoid( 90.0f, 2000.0f, 300.0f, 0.0f, 0.0f, false );
          interrupt->setSideSensorControl(true);
          while( trape->status() == false );
          interrupt->setSideSensorControl(false);
        } else if( next_dir == Right) {
          maze->updatePosition(&pos, next_dir);
          trape->makeTrapezoid( 90.0f, 2000.0f, 300.0f, 0.0f, 0.0f, false );
          interrupt->setSideSensorControl(true);
          while( trape->status() == false );
          interrupt->setSideSensorControl(false);
          trape->makeTrapezoid( -TURN_90, 1000.0f, 200.0f, 0.0f, 0.0f, true );
          while( trape->status() == false );
          trape->makeTrapezoid( 90.0f, 2000.0f, 300.0f, 0.0f, 0.0f, false );
          interrupt->setSideSensorControl(true);
          while( trape->status() == false );
          interrupt->setSideSensorControl(false);
        } else if( next_dir == Rear){
          maze->updatePosition(&pos, next_dir);
          trape->makeTrapezoid( 90.0f, 2000.0f, 300.0f, 0.0f, 0.0f, false );
          interrupt->setSideSensorControl(true);
          while( trape->status() == false );
          interrupt->setSideSensorControl(false);
          trape->makeTrapezoid( TURN_180, 1000.0f, 200.0f, 0.0f, 0.0f, true );
          while( trape->status() == false );
          trape->makeTrapezoid( 90.0f, 2000.0f, 300.0f, 0.0f, 0.0f, false );
          interrupt->setSideSensorControl(true);
          while( trape->status() == false );
          interrupt->setSideSensorControl(false);
        }
      }
      trape->makeTrapezoid( 90.0f, 2000.0f, 300.0f, 0.0f, 0.0f, false );
      while( trape->status() == false );
    } else if(mode_count == 6){
      sleep(1);
      trape->makeTrapezoid( 360.0f, 2000.0f, 300.0f, 0.0f, 0.0f, false );
      interrupt->setSideSensorControl(true);
      while( trape->status() == false );
      interrupt->setSideSensorControl(false);
    } else if(mode_count == 7){
      sleep(1);
      trape->makeTrapezoid( TURN_90, 1000.0f, 200.0f, 0.0f, 0.0f, true );
      while( trape->status() == false );
    } else if(mode_count == 8){
      sleep(1);
      trape->makeTrapezoid( TURN_180, 1000.0f, 200.0f, 0.0f, 0.0f, true );
      while( trape->status() == false );
    }
    mode_count = 0;
  } 

}

