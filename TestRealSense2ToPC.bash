#!/bin/bash
#RealSense2ToPCのテスト

#スクリプトのあるディレクトリへ移動
cd `dirname $0 `

#ネーミングサービスの確認
rtls /localhost > /dev/null
if [ $? -ne 0 ] 
then
  echo "ネーミングサーバが見つかりません"
  read -p "Enterを押してください"
  exit
fi

python=python3
pythonScript=SetCommand

#コンポーネントの起動
./RealSense2ToPC.bash
../PointCloudViewer/PointCloudViewer.bash
${python} ./${pythonScript}.pyw > /dev/null 2>&1 &

#コンポーネント名を変数化
s=/localhost/RealSense2ToPC0.rtc
v=/localhost/PointCloudViewer0.rtc
i=/localhost/${pythonScript}0.rtc
rtcs=(${s} ${v} ${i})

#コンポーネント起動待ち
for c in ${rtcs[*]}
do
  while :
  do
    echo "${c}の起動待ち"
    sleep 1
    rtls ${c} > /dev/null 2>&1
    if [ $? -eq 0 ]
    then
      break
    fi
  done
done

#接続
rtcon ${s}:pc ${v}:pc
rtcon ${i}:command ${s}:command

#アクティベート
rtact ${rtcs[*]}

while :
do
  read -p "終了しますか？ (y/n): " ans
  if [ ${ans} = "y" ] 
  then
    break
  fi
done

#ディアクティベート
rtdeact ${rtcs[*]}

#終了（rtexitは，引数を一つずつ）
for c in ${rtcs[*]}
do
  rtexit ${c}
done

pkill -f ./${pythonScript}.pyw
