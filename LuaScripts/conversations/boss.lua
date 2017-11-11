dialogues = {
   function()
      clayer:changeBgp('background/Koumakan_Library.jpg')
         :changeBgm('bgm/bgm020.mp3')

      clayer:changeSpeakerColor('Remilia Scarle', {r=148, g=100, b=161})
      clayer:changeSpeakerColor('Flandore Scarle', {r=242, g=169, b=74})
      clayer:changeSpeakerColor('Hakurei Reimu',   {r=0xfe, g=0x46, b=0x2b})
      clayer:changeSpeakerColor('Udongein Inaba',  {r=0x233, g=0x75, b=109})
      clayer:changeSpeakerColor('Izayoi Sakuya',   {r=212, g=182, b=196})
      
      clayer:changeCharacter('RIGHT',
                             'Udongein Inaba',
                             'avatar/udong_pro_7.png')
         :changeText('Udongein Inaba',
                     '呜呜呜呜~')
   end,

   function()
      clayer:changeCharacter('RIGHT',
                             'Izayoi Sakuya',
                             'avatar/sakuya_pro_7.png',
                             {r=212, g=182, b=196},
                             'wordArt/reimu_name_wordart.png')
         :changeText('Izayoi Sakuya',
                     '对不起，大小姐，没能拦住她们......')
   end,

   function()
      clayer:changeCharacter('RIGHT',
                             'Remilia Scarle',
                             'avatar/remilia_pro_4.png')
         :changeText('Remilia Scarle', '欢迎来到红魔馆，博丽神社的巫女')
   end,

   function()
      clayer:changeCharacter('LEFT',
                             'Hakurei Reimu',
                             'avatar/reimu_pro_7.png')
         :changeText('Hakurei Reimu', '我来干嘛的，想必你也已经知道了，红魔的吸血鬼姐妹。')
   end,

   function()
      clayer:changeCharacter('RIGHT',
                             'Remilia Scarle',
                             'avatar/remilia_pro_5.png')
         :changeText('Remilia Scarle', '啊拉，到底为何事呢？')
      -- TODO
   end,

   function()
      clayer:changeCharacter('MIDDLE',
                             'Flandore Scarle',
                             'avatar/flandore_pro_3.png')
         :changeText('Flandore Scarle', '姐姐，姐姐，她们是来陪芙兰玩耍的吗？')
   end,
   
   function()
      clayer:changeCharacter('RIGHT',
                             'Remilia Scarle',
                             'avatar/remilia_pro_2.png')
         :changeText('Remilia Scarle', '是啊，她们是来陪芙兰玩耍的呀，小心点不要玩坏了哦。')
   end,

   function()
      clayer:changeCharacter('MIDDLE',
                             'Flandore Scarle',
                             'avatar/flandore_pro_3.png')
         :changeText('Flandore Scarle', '好的，姐姐')
   end,

   function()
      clayer:changeCharacter('MIDDLE',
                             'Flandore Scarle',
                             'avatar/flandore_pro_6.png')
         :changeText('Flandore Scarle', '看起来不会的......')
   end,

   function()
   end,
}
