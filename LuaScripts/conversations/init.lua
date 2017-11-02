function nDialogues (dialoguestbl)
   return #dialoguestbl
end

function playDialogue (dialoguestbl, i)
   (dialoguestbl[i])()
end

clayer = tg.ConversationLayer:currInstance()
