dialogues = {
   function()
      clayer:changeBgp('background/Forest_of_Magic.png')
         :changeBgm('bgm/sample.mp3')

      clayer:changeSpeakerColor('Kirisame Marisa', {r=0xff, g=0xf8, b=0x7c})
      clayer:changeSpeakerColor('Hakurei Reimu',   {r=0xfe, g=0x46, b=0x2b})

         :changeCharacter('LEFT',
                          'Kirisame Marisa',
                          'avatar/marisa_pro_6.png',
                          {r=0xff, g=0xf8, b=0x7c},
                          'wordArt/marisa_name_wordart.png')
         :changeText('Kirisame Marisa',
                     '哟，灵梦。我在这附近抓住了一只冰之妖精（⑨）哦')
         :playSoundEffect('se/happy.wav')
         :playScreenEffect('SHAKE')
   end,

   function()
      clayer:changeCharacter('RIGHT',
                             'Hakurei Reimu',
                             'avatar/reimu_pro_9.png',
                             {r=0xfe, g=0x46, b=0x2b},
                             'wordArt/reimu_name_wordart.png')
         :changeText('Hakurei Reimu',
                     '啊，欢迎光临，魔理沙。妖精这玩意啊，真是一个不注意就冒出来了呢')
   end,

   function()
      clayer:changeCharacter('RIGHT',
                             'Hakurei Reimu',
                             'avatar/reimu_pro_7.png')
         :changeText('Hakurei Reimu', '很冷啊，快把门关上好。')
         :playSoundEffect('se/surprise.wav')
   end,

   function()
      clayer:changeCharacter('LEFT',
                             'Kirisame Marisa',
                             'avatar/marisa_pro_1.png')
         :changeText('Kirisame Marisa', '看看啊，明明已经到春天了，外面却还是这副大雪天。'..
                           '冬之妖精什么的，妖怪什么的，你也是时候认清现实了，这是异变阿！",')
   end,

   function()
      clayer:changeCharacter('RIGHT',
                             'Hakurei Reimu',
                             'avatar/reimu_pro_8.png')
         :changeText('Hakurei Reimu', '今年的春天只是晚来了会而已......')
   end,

   function()
      clayer:changeCharacter('LEFT',
                             'Kirisame Marisa',
                             'avatar/marisa_pro_9.png')
         :changeText('Kirisame Marisa', '不对，一定有妖精在作祟。'..
                           '消除异变和驱除妖怪不是你这个巫女的工作吗？')
   end,

   function()
      clayer:changeCharacter('RIGHT',
                             'Hakurei Reimu',
                             'avatar/reimu_pro_9.png')
         :changeText('Hakurei Reimu', '（喝茶）')
   end,

   function()
      clayer:changeCharacter('LEFT',
                             'Kirisame Marisa',
                             'avatar/marisa_pro_5.png')
         :changeText('Kirisame Marisa', '啊，是吗。这样的话这次的异变我自己一个人来解决好了。'..
                        '别最后才若无其事的出来说『对不起啊，这果然是异变呢』。')
   end,

   function()
      clayer:changeCharacter('LEFT', '')
   end,

   function()
      clayer:changeCharacter('RIGHT',
                             'Hakurei Reimu',
                             'avatar/reimu_pro_1.png')
   end,
}
