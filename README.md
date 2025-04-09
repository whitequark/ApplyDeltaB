# ApplyDeltaB

The _ApplyDeltaB_ tool can be used to apply forward and reverse delta patches in the format used by `msdelta.dll`. This is the format used by Windows Update.


Usage
-----

```
Usage: ApplyDeltaB.exe [Source] [Delta] [Target]
```

You can use the WinSxS store to examine updates that have been applied to the current Windows installation. For example, if you are interested in the changes a Windows security update has introduced to a file `rndismp6.sys`, run in PowerShell:

```
PS > Get-ChildItem -Recurse c:\Windows\WinSxS |? { $_.Name -match "rndismp6" }


    Directory: C:\Windows\WinSxS\amd64_dual_netrndis.inf_31bf3856ad364e35_10.0.19041.4291_none_ec97fac579743e91


Mode                 LastWriteTime         Length Name
----                 -------------         ------ ----
-a----        2024-06-15     19:25          41472 rndismp6.sys


    Directory: C:\Windows\WinSxS\amd64_dual_netrndis.inf_31bf3856ad364e35_10.0.19041.4291_none_ec97fac579743e91\f


Mode                 LastWriteTime         Length Name
----                 -------------         ------ ----
-a----        2024-06-07     07:36           5035 rndismp6.sys


    Directory: C:\Windows\WinSxS\amd64_dual_netrndis.inf_31bf3856ad364e35_10.0.19041.4291_none_ec97fac579743e91\r


Mode                 LastWriteTime         Length Name
----                 -------------         ------ ----
-a----        2024-06-15     19:25           9278 rndismp6.sys

PS > $patch = "C:\Windows\WinSxS\amd64_dual_netrndis.inf_31bf3856ad364e35_10.0.19041.4291_none_ec97fac579743e91"
PS > .\ApplyDeltaB.exe $patch\rndismp6.sys $patch\r\rndismp6.sys rndismp6.prev.sys
PS > Copy-Item $patch\rndismp6.sys rndismp.curr.sys
```

Now, `rndismp6.prev.sys` contains the pre-KB5036892 version of `rndismp6.sys`, and `rndismp.curr.sys` contains the installed version of this driver. From this point on, you can use tools like [BinDiff] to examine the difference.

[BinDiff]: https://github.com/google/bindiff


License
-------

[0-clause BSD](LICENSE.txt)