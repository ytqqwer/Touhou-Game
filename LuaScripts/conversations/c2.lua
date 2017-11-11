dialogues = {
   function()
      clayer:changeBgp('background/Forest_of_Magic.png')
         :changeBgm('bgm/sample.mp3')

      clayer:changeSpeakerColor('Kirisame Marisa', {r=0xff, g=0xf8, b=0x7c})
      clayer:changeSpeakerColor('Hakurei Reimu',   {r=0xfe, g=0x46, b=0x2b})
      clayer:changeSpeakerColor('Remilia Scarle', {r=148, g=100, b=161})
      clayer:changeSpeakerColor('Flandore Scarle', {r=242, g=169, b=74})
      clayer:changeSpeakerColor('Hakurei Reimu',   {r=0xfe, g=0x46, b=0x2b})
      clayer:changeSpeakerColor('Udongein Inaba',  {r=0x233, g=0x75, b=109})
      clayer:changeSpeakerColor('Izayoi Sakuya',   {r=212, g=182, b=196})

         :changeCharacter('LEFT',
                          'Izayoi Sakuya',
                          'avatar/sakuya_pro_6.png',
                          {r=0xff, g=0xf8, b=0x7c},
                          'wordArt/marisa_name_wordart.png')
         :changeText('Izayoi Sakuya',
                     '打扰了。你好...哎呀，真的还在呢')
   end,

   function()
      clayer:changeCharacter('RIGHT',
                             'Hakurei Reimu',
                             'avatar/reimu_pro_1.png',
                             {r=0xfe, g=0x46, b=0x2b},
                             'wordArt/reimu_name_wordart.png')
         :changeText('Hakurei Reimu',
                     '为什么大家都不从正门进呢')
   end,

   function()
      clayer:changeCharacter('RIGHT',
                             'Hakurei Reimu',
                             'avatar/reimu_pro_3.png')
         :changeText('Hakurei Reimu', '这不是吸血鬼那边的女仆么，今天一个人来的么。')
   end,

   function()
      clayer:changeCharacter('LEFT',
                             'Izayoi Sakuya',
                             'avatar/sakuya_pro_2.png')
         :changeText('Izayoi Sakuya', '大小姐并未前来，我是来传话的『这样放任妖怪乱来真的好吗？』')
   end,

   function()
      clayer:changeCharacter('RIGHT',
                             'Hakurei Reimu',
                             'avatar/reimu_pro_5.png')
         :changeText('Hakurei Reimu', '真的是多管闲事')
   end,

   function()
      clayer:changeCharacter('LEFT',
                             'Izayoi Sakuya',
                             'avatar/sakuya_pro_9.png')
         :changeText('Izayoi Sakuya', '也罢，你所考虑的事我也不是不理解')
   end,

   function()
      clayer:changeCharacter('RIGHT',
                             'Hakurei Reimu',
                             'avatar/reimu_pro_7.png')
         :changeText('Hakurei Reimu', '什么嘛')
   end,

   function()
      clayer:changeCharacter('LEFT',
                             'Izayoi Sakuya',
                             'avatar/sakuya_pro_9.png')
         :changeText('Izayoi Sakuya', '维持幻想乡的平衡是巫女的工作吧')
   end,

   function()
      clayer:changeCharacter('RIGHT',
                             'Hakurei Reimu',
                             'avatar/reimu_pro_8.png')
         :changeText('Hakurei Reimu', '说...说的也是呢，决定该怎么做是我的工作...')
   end,

   function()
      clayer:changeCharacter('LEFT',
                             'Izayoi Sakuya',
                             'avatar/sakuya_pro_2.png')
         :changeText('Izayoi Sakuya', '想说的是，话已经传到您了。黑白魔法使已经去了冥界'..
                        '死了的话，不知道会不会成为亡灵呢')
   end,

   function()
      clayer:changeCharacter('RIGHT',
                             'Hakurei Reimu',
                             'avatar/reimu_pro_5.png')
         :changeText('Hakurei Reimu', '真是不让人省心')
   end,
}
