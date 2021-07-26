const path = require('path')
const download = require('download')
const tar = require('tar')
const fs = require('fs')
const extract = require('extract-zip')
const { logger } = require('just-task')

module.exports = ({
  fetchUrl,
  extractPath,
  temporaryPath,
  platform = process.platform,
  arch = process.arch
}) => {
  return new Promise((resolve, reject) => {
    logger.info(`Downloading file from: ${fetchUrl} to ${temporaryPath}`)
    if (fs.existsSync(temporaryPath)) {
      fs.rmdirSync(temporaryPath, { recursive: true })
    }
    if (fs.existsSync(extractPath)) {
      fs.rmdirSync(extractPath, { recursive: true })
    }
    // if (fs.existsSync(extractPath.replace('nertc_sdk', 'sdk'))) {
    //   fs.rmdirSync(extractPath.replace('nertc_sdk', 'sdk'), { recursive: true })
    // }
    download(fetchUrl, extractPath, { extract: true, strip: 1 }).then(() => {
      if (platform === 'win32') {
        let copyArch
        if (arch === 'ia32') {
          copyArch = 'x86'
        } else if (arch === 'x64') {
          copyArch = 'x64'
        } else {
          reject(new Error('Unsupported arch.'))
        }
        const dllSrcPath = path.join(extractPath, `dll/${copyArch}`)
        const libSrcPath = path.join(extractPath, `lib/${copyArch}`)
        const dlls = fs.readdirSync(dllSrcPath)
        logger.info(dlls)
        dlls.map((dll) => {
          logger.info(`Copy file from ${dll} to ${extractPath}/dll`)
          fs.copyFileSync(path.join(dllSrcPath, dll), path.join(extractPath, `dll/${dll}`))
        })
        const libs = fs.readdirSync(libSrcPath)
        logger.info(libs)
        libs.map((lib) => {
          logger.info(`Copy file from ${lib} to ${extractPath}/lib`)
          fs.copyFileSync(path.join(libSrcPath, lib), path.join(extractPath, `lib/${lib}`))
        })
      } else if (platform === 'darwin') {
        fs.renameSync(path.join(extractPath.replace('nertc_sdk', ''), 'sdk'), extractPath)
      } else {
        return reject(new Error('Unsupported platform.'))
      }
      resolve()
    }).catch(err => {
      console.log(err);
      reject(err)
    })
  })
}
