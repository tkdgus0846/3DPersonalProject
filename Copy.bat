
attrib				-R			.\Reference\Headers\*.*
xcopy				/y/s		.\Engine\Public\*.*					.\Reference\Headers\
attrib				+R			.\Reference\Headers\*.*


xcopy				/y/s		.\Engine\ThirdPartyLib\*.*			.\Reference\Librarys\
xcopy				/y/s		.\Client\Bin\ShaderFiles\*.*		.\Tool\Bin\ShaderFiles\
xcopy				/y/s		.\Engine\Bin\ShaderFiles\*.*		.\Client\Bin\ShaderFiles\
xcopy				/y/s		.\Engine\Bin\ShaderFiles\*.*		.\Tool\Bin\ShaderFiles\

xcopy				/y			.\Engine\Bin\Engine.lib				.\Reference\Librarys\
xcopy				/y			.\Engine\Bin\Engine.dll				.\Client\Bin\
xcopy				/y			.\Engine\Bin\Engine.dll				.\Tool\Bin\
xcopy				/y			.\Engine\Bin\Engine.dll				.\ModelLoader\Bin\
