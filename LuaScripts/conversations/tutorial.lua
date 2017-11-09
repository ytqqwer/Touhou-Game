dialogues = {
   function()
      clayer:changeBgp('tutorial/bgp.jpg')
         :setDialogueInterval(100)
         :changeBgm('bgm/bgm003.mp3')

         :changeCharacter('LEFT',
                          '灵梦',
                          '',
                          {r=0xfe, g=0x46, b=0x2b},
                          '')
         :changeText('灵梦',
                     'Hi, 我是灵梦哟！在一起玩耍之前，有一些操作技巧要告诉你。')
   end,

   function()
      clayer:changeCharacter('MIDDLE',
                             'mainmenu',
                             'tutorial/mainmenu.png',
                             {r=0xff, g=0xff, b=0xff},
                             '',
                             {x=0.3, y=0.4})
         :changeText("灵梦",
                     '打开游戏进入这个界面 —— '..
                        '在这个界面里面可以选择再和上次的小伙伴们一起玩耍，或者开始新的旅程哦 (›´ω`‹ )')
   end,

   function()
      clayer:changeCharacter('MIDDLE',
                             'mainmenu',
                             'tutorial/mainmenu.png',
                             {r=0xff, g=0xff, b=0xff},
                             '',
                             {x=0.3, y=0.4})
         :changeText("灵梦",
                     '点击 新游戏/继续游戏/载入游戏 进入主界')
   end,
   
   function()
      clayer:changeCharacter('MIDDLE',
                             'homescene',
                             'tutorial/homescene.png',
                             {r=0xff, g=0xff, b=0xff},
                             '',
                             {x=0.3, y=0.4})
         :changeText("灵梦",
                     '这里可以选择 立即出发 或者选择 整备一下')
   end,

   function()
      clayer:changeCharacter('MIDDLE',
                             'homescene3',
                             'tutorial/homescene3.png',
                             {r=0xff, g=0xff, b=0xff},
                             '',
                             {x=0.3, y=0.4})
         :changeText("灵梦",
                     '在不同的地点会有不同的人物和对话可供选择，玩累了记得和下方的小可爱们聊聊天哟')
   end,

   function()
      clayer:changeCharacter('MIDDLE',
                             'homescene4',
                             'tutorial/homescene4.png',
                             {r=0xff, g=0xff, b=0xff},
                             '',
                             {x=0.3, y=0.4})
         :changeText("灵梦",
                     '在不同的地点会有不同的人物和对话可供选择，玩累了记得和下方的小可爱们聊聊天哟')
   end,
   
   function()
      clayer:changeCharacter('MIDDLE',
                             '3move',
                             'tutorial/3move.png',
                             {r=0xff, g=0xff, b=0xff},
                             '',
                             {x=0.3, y=0.4})
         :changeText("灵梦",
                     '让我们马上开始！'..
                        '屏幕分为上下三部分。点击 U 部分可使人物跳动，点击 LB 部分可使人物向左走；'..
                        '点击 RB 部分可使人物向右走')
   end,

   function()
      clayer:changeCharacter('MIDDLE',
                             'jump-attack',
                             'tutorial/jump-attack.png',
                             {r=0xff, g=0xff, b=0xff},
                             '',
                             {x=0.3, y=0.4})
         :changeText("灵梦",
                     '屏幕右边是切换攻击方式和 dash 按钮，配合他们可以更快的打击，更快的移动哦')
   end,

   function()
      clayer:changeCharacter('MIDDLE',
                             'item-card',
                             'tutorial/item-card.png',
                             {r=0xff, g=0xff, b=0xff},
                             '',
                             {x=0.3, y=0.4})
         :changeText("灵梦",
                     '遇到强劲的敌人\n'..
                        '可以使用屏幕下方的道具，狠狠打击他们（还可以用来保命 |･ω･｀)')
   end,

   function()
      clayer:changeCharacter('MIDDLE',
                             'boss',
                             'tutorial/boss.png',
                             {r=0xff, g=0xff, b=0xff},
                             '',
                             {x=0.3, y=0.4})
         :changeText("灵梦",
                     'Boss 有时候是一个，有时候是两个，有时候更多')
   end,

   function()
      clayer:changeCharacter('MIDDLE',
                             'hp-mana',
                             'tutorial/hp-mana.png',
                             {r=0xff, g=0xff, b=0xff},
                             '',
                             {x=0.3, y=0.4})
         :changeText("灵梦",
                     '战斗过程中要时刻关注血条蓝条变化，自己或 Boss 被打败了回合就结束了')
   end,

   function()
      clayer:changeCharacter('MIDDLE',
                             'conversation',
                             'tutorial/conversation.png',
                             {r=0xff, g=0xff, b=0xff},
                             '',
                             {x=0.3, y=0.4})
         :changeText("灵梦",
                     '回合结束时很大几率会触发一段对话，和被打败的 Boss 聊一聊吧！')
   end,

   function()
      clayer:changeText("灵梦",
                     '教程就到这里了，来和灵梦一起愉快玩耍吧 (´｡･v･｡｀)')
   end,
}
