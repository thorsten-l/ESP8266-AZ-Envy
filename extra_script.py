import os
from shutil import copyfile

Import("env")
pioEnv = env['PIOENV']

def copyFirmware(source, target, env):
  print ( "------------------------------------------------------------------------------" )
  buildFlags = env.ParseFlags(env['BUILD_FLAGS'])
  cppDefines = buildFlags.get( "CPPDEFINES" )

  appVersion = "0.0.0"

  i = 0

  while i < len(cppDefines):
    if len(cppDefines[i]) == 2:
      if "APP_VERSION" == cppDefines[i][0]:
        appVersion = cppDefines[i][1]
        l = len(appVersion)
        appVersion = appVersion[1:l-1]
        break
    i += 1

  firmwarePath = os.path.abspath(os.path.join( os.path.join( target[0].path, os.pardir), ( "firmware-%s.%s.bin" % ( appVersion, pioEnv ))))
 
  print ( "PIOENV = " + pioEnv )
  print ( "copy firmware to file:" )
  print ( firmwarePath )
  copyfile( target[0].path, firmwarePath )
  print ( "size=%d bytes" % os.path.getsize( firmwarePath ))

env.AddPostAction( "$BUILD_DIR/firmware.bin", copyFirmware )
env.Append(CPPDEFINES=[("PIOENV", "\\\"" + env['PIOENV'] + "\\\"")])
