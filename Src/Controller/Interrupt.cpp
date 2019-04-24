/**
 * Interrupt.cpp
 * @author yuta seya
 * @date 2019 4.2
*/
#include "Interrupt.h"

#include <thread>
#include <iostream>
#include <unistd.h>


/**
 * @brief 割り込みクラスのコンストラクタ
 * @param なし
 * @return　なし
*/
Interrupt::Interrupt()
{
  if ( trape == nullptr ){
    trape = Trapezoid::getInstance();
  }

  if ( target_trans == nullptr ){
    target_trans = new TargetGenerator();
  }
}

/**
 * @brief 割り込みクラスのデストラクタ
 * @param なし
 * @return　なし
*/
Interrupt::~Interrupt()
{

}

/**
 * @brief 割り込みの処理を行う
 * @param なし
 * @return　なし
*/
void Interrupt::processing()
{
  while( 1 ){
    std::cout << "割り込み処理読んだ？" << std::endl;
    usleep( 1000000 );
  }
}

/**
 * @brief コントロールフラグのセットをする
 * @param なし
 * @return　なし
*/
void Interrupt::setSensor( bool flag )
{
  sensor = flag;
}

/**
 * @brief センサフラグのセットをする
 * @param なし
 * @return　なし
*/
void Interrupt::setControl( bool flag )
{
  control = flag;
}
