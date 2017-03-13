#!/usr/bin/env python
# -*- coding: utf-8 -*-

import grp
import pwd
import os

def fFind(_mLevel,_mRootName,_mOtherRootName):
    _mNameList  = os.listdir(_mRootName)
    _mNameList1 = os.listdir(_mOtherRootName)
    _mNameList  = sorted(_mNameList)
    _mNameList1 = sorted(_mNameList1)
    _mFindIndex = 0
    for _mItem in _mNameList :
        if _mItem != _mNameList1[_mFindIndex] :
            print "[Error]"
            exit(1)
        _mFindIndex += 1 
        if _mLevel == 0:
            if _mItem == "dev":
                continue
            if _mItem == "proc":
                continue
            if _mItem == "sys":
                continue
            if _mItem == "tmp":
                continue

        _mName = "{}{}".format(_mRootName,_mItem)
        _mOtherName = "{}{}".format(_mOtherRootName,_mItem)

        if os.path.islink(_mName):
            if not os.path.islink(_mOtherName):
                print "[Error] {} {} \nsymbol link is not same".format(_mName,_mOtherName)
                #exit(1)
        else:
            _mStatInfo = os.stat(_mName)
            _mStatInfo1 = os.stat(_mOtherName)
            if _mStatInfo.st_uid != _mStatInfo1.st_uid :
                print "[Error] {} {} \nuid ({} {}) is not same".format(_mName,_mOtherName,_mStatInfo.st_uid,_mStatInfo1.st_uid)
                #exit(1)                
            if _mStatInfo.st_gid != _mStatInfo1.st_gid :
                print "[Error] {} {} \ngid ({} {})is not same".format(_mName,_mOtherName,_mStatInfo.st_gid,_mStatInfo1.st_gid)
                #exit(1)  
            if _mStatInfo.st_mode != _mStatInfo1.st_mode :
                print "[Error] {} {} \nmode ({} {})is not same".format(_mName,_mOtherName,_mStatInfo.st_mode,_mStatInfo1.st_mode)
                #exit(1)  
            #print _mStatInfo.st_uid , _mStatInfo.st_gid , _mStatInfo.st_mode
            if os.path.isdir(_mName) :
                #print "{} Yes".format(_mName)
                fFind(_mLevel+1,"{}/".format(_mName),"{}/".format(_mOtherName))

_mDstPath = "{}/dst/".format(os.getcwd())
_mSrcPath = "{}/src/".format(os.getcwd())
fFind(0,_mDstPath,_mSrcPath )

