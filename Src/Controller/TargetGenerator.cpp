/**
 * TargetGenerator.h
 * @author yuta seya
 * @date 2019 4.2
*/

#include "TargetGenerator.h"

/**
 * @brief 目標値の生成クラスのコンストラクタ
 * @param なし
 * @return　なし
*/
TargetGenerator::TargetGenerator()
{
  trape = Trapezoid::getInstance();
}

/**
 * @brief 目標値の生成クラスのコンストラクタ
 * @param なし
 * @return　なし
*/
TargetGenerator::~TargetGenerator()
{

}

/**
 * @brief ステップ周波数を返す
 * @param なし
 * @return　なし
*/
void TargetGenerator::getStepFrequency( int *left, int *right, bool back_right )
{
  // step_sensorの足し引きは暫定
  if ( back_right ){
    *left += -1 * (step_vel - step_sensor);
    *right += -1 * (step_vel + step_sensor);
  } else {
    *left += step_vel;
    *right += step_vel + step_sensor;
  }

}

/**
 * @brief 速度からステップ周波数を計算する
 * @param なし
 * @return　なし
*/
void TargetGenerator::calcStepFrequency()
{
  float velocity = 0.0f;
  float frequency = 0.0f;

  velocity = trape->getNextVelocity();

  // v_step[step/sec] = v[mm/s] / d[mm/step]
  // v_step パルス周波数
  frequency = velocity / STEP_DISTANCE_CONST;

  step_vel = int(frequency);
}