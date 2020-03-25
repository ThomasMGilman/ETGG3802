When embedding python, there are a few key modules (sys, os, encodings, filesystem, etc.) that need to be in place.  What's more, 
they need to match 100% the version of python we link against in our program that embeds python (including 32 vs 64-bit).  This
is a problem if the user has a different version of python installed (or doesn't have python installed at all).  The fix is to
put these python38.zip files in your run-time path (I added them to bin/Debug and bin/Release) and then before calling Py_Initialize,
call Py_SetPath.  For me, the call looked like this:

Py_SetPath(L"./python38.zip");

And then we should be all set!