@echo off

(call )

setlocal EnableExtensions EnableDelayedExpansion 1>nul 2>nul

set \n=^^^
%= DO NOT REMOVE THIS LINE =%
^
%= DO NOT REMOVE THIS LINE =%
%= DO NOT REMOVE THIS LINE =%

set \^^=^^^^

if not [%ErrorLevel%] == [0] (
    echo;%\n%^
BUILD.BAT @ EXTENSIONS ARE NOT AVAILABLE%\n%%\n%^
  setlocal EnableExtensions EnableDelayedExpansion%\n%^
           ^
%\^%%\^%%\^%%\^%%\^%%\^%%\^%%\^%%\^%%\^%%\^%%\^%%\^%%\^%%\^%%\^% ^
%\^%%\^%%\^%%\^%%\^%%\^%%\^%%\^%%\^%%\^%%\^%%\^%%\^%%\^%%\^%%\^%%\^%%\^%%\^%%\^%%\^%%\^% 1>&2

    endlocal
    exit /b 1
)

call :init-build-flags "/link /clean /run /help /debug /cdb /license" "/l /c /r /h /d "" """ "%*"

if [!_flag.is_help!] == [1] (
    if [!_flag.is_debug!] == [1] (
        echo;
    )

    echo;%\n%^
BUILD.BAT [/L[INK]] [/C[LEAN]] [/R[UN]] [/H[ELP]] [/D[EBUG]] [/CDB] [/LICENSE]%\n%%\n%^
  /L[INK]       Create symlinks to header files specified by the property.%\n%^
  /C[LEAN]      Clean files and directories produced by the compiler.%\n%^
  /R[UN]        Run an output executable file produced by the compiler.%\n%^
  /H[ELP]       Display this help message and finish execution of the build.%\n%^
  /D[EBUG]      Display debugging messages during execution of the build.%\n%^
  /CDB          Write compilation options to `compile_flags.txt` file and exit.%\n%^
  /LICENSE      Display the license text of this `build.bat` file and exit.%\n%%\n%%\n%^
.HARMONY~BUILD ^<COMPILER^> [OPTIONS] ^<OUTPUT^> ^<INCLUDE^> [SOURCES] [HEADERS] [LIBRARIES] [BINARIES]%\n%%\n%^
  COMPILER      Specifies path to an executable program used for compilation of source files.%\n%^
  OPTIONS       Specifies paths to files containing compilation options or a set of themself.%\n%^
  OUTPUT        Specifies path with file name to an output file produced by the compiler.%\n%^
  INCLUDE       Specifies path to an include directory used by the link process and the flag.%\n%^
  SOURCES       Specifies paths to source files used by the compiler to produce a final program.%\n%^
  HEADERS       Specifies paths to header files used by the link process to create symlinks.%\n%^
  LIBRARIES     Specifies paths to library files used by the final program as dependencies.%\n%^
  BINARIES      Specifies paths to binary files used by the final program as dependencies.

    endlocal
    exit /b 0
)

if [!_flag.is_license!] == [1] (
    if [!_flag.is_debug!] == [1] (
        echo;
    )

    echo;%\n%^
MIT License%\n%%\n%^
Copyright ^(c^) 2024 anominy%\n%%\n%^
Permission is hereby granted, free of charge, to any person obtaining a copy%\n%^
of this software and associated documentation files ^(the "Software"^), to deal%\n%^
in the Software without restriction, including without limitation the rights%\n%^
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell%\n%^
copies of the Software, and to permit persons to whom the Software is%\n%^
furnished to do so, subject to the following conditions:%\n%%\n%^
The above copyright notice and this permission notice shall be included in all%\n%^
copies or substantial portions of the Software.%\n%%\n%^
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR%\n%^
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,%\n%^
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE%\n%^
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER%\n%^
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,%\n%^
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE%\n%^
SOFTWARE.

    endlocal
    exit /b 0
)

call :init-build-properties .harmony~build

if [!_flag.is_cdb!] == [1] (
    call :init-compiler-options !_config.options!
    set _opts=!__last_init_compiler_options_result: =%\n%!

    (echo;!_opts!) 1>compile_flags.txt 2>nul

    endlocal
    exit /b 0
)

if [!_config.compiler!] == [] (
    set _property_error_0=!_property_error_0! ^
compiler=

    set _property_error_1=!_property_error_1! ^
%\^%%\^%%\^%%\^%%\^%%\^%%\^%%\^%%\^%%\^%%\^%%\^%%\^%%\^%%\^%%\^%%\^%%\n%
)

if [!_config.output!] == [] (
    set _property_error_0=!_property_error_0! ^
output=

    set _property_error_1=!_property_error_1! ^
%\^%%\^%%\^%%\^%%\^%%\^%%\^%%\^%%\^%%\^%%\^%%\^%%\^%%\n%
)

if [!_config.include!] == [] (
    set _property_error_0=!_property_error_0! ^
include=

    set _property_error_1=!_property_error_1! ^
%\^%%\^%%\^%%\^%%\^%%\^%%\^%%\^%%\^%%\^%%\^%%\^%%\^%%\^%%\^%%\n%
)

if not [!_property_error_0!] == [] (
    if [!_flag.is_debug!] == [1] (
        echo;
    )

    set _property_error_0=!_property_error_0:~1!
    set _property_error_1=!_property_error_1:~1!

    echo;%\n%^
.HARMONY~BUILD @ REQUIRED PROPERTIES ARE EMPTY%\n%%\n%^
  ^
!_property_error_0!%\n%^
  ^
!_property_error_1!

    endlocal
    exit /b 1
)

if [!_flag.is_clean!] == [1] (
    call :clean "!_config.output!"
)

if [!_flag.is_link!] == [1] (
    call :link "!_config.include!" !_config.headers!
)

call :init-compiler-options !_config.options!
set _opts=!__last_init_compiler_options_result!

call :init-source-files !_config.sources!
set _srcs=!__last_init_source_files_result!

call :init-library-files !_config.libraries!
set _libs=!__last_init_library_files_result!

call :init-compiler-output !_config.output! !_config.binaries!
set _outp=!__last_init_compiler_output_result!

cmd /c !_config.compiler! !_opts! !_srcs! !_libs! -o !_outp!

if [!_flag.is_run!] == [1] (
    call :run "!_outp!" %*
)

endlocal
exit /b 0


:run
setlocal
set __exe_path="%~1"
set __exe_args=

call :tail %*
set __local_args=!__last_tail_result!

:run-loop
for /f "usebackq eol= tokens=1* delims=/" %%a in ('!__local_args!') do (
    set __exe_args=%%a

    set __local_args=%%b
    goto :run-loop
)

for /f "usebackq eol= tokens=1* delims= " %%a in ('!__exe_args!') do (
    set __exe_args=%%b
)

cmd /c !__exe_path! !__exe_args!
endlocal
exit /b


:init-compiler-output
setlocal
set __out_path=%~dp1
set __exe_name=%~nx1

call :tail %*
set __binaries=!__last_tail_result!

md "!__out_path!" 1>nul 2>nul && (
:init-compiler-output-loop
    for /f "usebackq eol= tokens=1* delims= " %%a in ('!__binaries!') do (
        set __binary_item=%%~a
        set __binary_path=%%~dpnxa

        set __bin_attr=%%~aa
        set __dir_attr=!__bin_attr:~,1!

        if not [!__binary_item!] == [] (
            if [!__dir_attr!] == [d] (
                for /f "usebackq eol= tokens=* delims=" %%a in (`where /r "!__binary_path!" *.dll 2^>nul`) do (
                    copy "%%a" "!__out_path!" 1>nul 2>nul
                )
            ) else (if [!__dir_attr!] == [-] (
                copy "!__binary_path!" "!__out_path!" 1>nul 2>nul
            ))
        )

        set __binaries=%%b
        goto :init-compiler-output-loop
    )
)

endlocal & set __last_init_compiler_output_result=%__out_path%%__exe_name%
copy nul "!__last_init_compiler_output_result!.local" 1>nul 2>nul
exit /b


:init-library-files
setlocal
set __libraries=%*

set __result=
:init-library-files-loop
for /f "usebackq eol= tokens=1* delims= " %%a in ('!__libraries!') do (
    set __library_item=%%~a
    set __library_path=%%~dpnxa

    set __lib_attr=%%~aa
    set __dir_attr=!__lib_attr:~,1!

    if not [!__library_item!] == [] (
        if [!__dir_attr!] == [d] (
            for /f "usebackq eol= tokens=* delims=" %%a in (`where /r "!__library_path!" *.lib 2^>nul`) do (
                set __result=!__result! "%%a"
            )
        ) else (if [!__dir_attr!] == [-] (
            set __result=!__result! "!__library_path!"
        ) else (
            set __result=!__result! "!__library_item!"
        ))
    )

    set __libraries=%%b
    goto :init-library-files-loop
)

if not [!__result!] == [] (
    set __result=!__result:~1!
)

endlocal & set __last_init_library_files_result=%__result%
exit /b


:init-source-files
setlocal
set __sources=%*

:init-source-files-loop
for /f "usebackq eol= tokens=1* delims= " %%a in ('!__sources!') do (
    set __source_item=%%~a
    set __source_path=%%~dpnxa

    set __src_attr=%%~aa
    set __dir_attr=!__src_attr:~,1!

    if not [!__source_item!] == [] (
        if [!__dir_attr!] == [d] (
            for /f "usebackq eol= tokens=* delims=" %%a in (`where /r "!__source_path!" *.c 2^>nul`) do (
                set __result=!__result! "%%a"
            )
        ) else (if [!__dir_attr!] == [-] (
            set __result=!__result! "!__source_path!"
        ))
    )

    set __sources=%%b
    goto :init-source-files-loop
)

if not [!__result!] == [] (
    set __result=!__result:~1!
)

endlocal & set __last_init_source_files_result=%__result%
exit /b


:init-compiler-options
setlocal
set __options=%*

:init-compiler-options-loop
for /f "usebackq eol= tokens=1* delims= " %%a in ('!__options!') do (
    set __option_item=%%~a

    set __opt_attr=%%~aa
    set __dir_attr=!__opt_attr:~,1!

    if not [!__option_item!] == [] (
        if [!__dir_attr!] == [-] (
            set __opt_text=
            for /f "usebackq eol= tokens=* delims=" %%a in ("!__option_item!") do (
                set __opt_text=!__opt_text! %%a
            )
            if not [!__opt_text!] == [] (
                set __opt_text=!__opt_text:~1!
            )
        ) else (
            set __opt_text=!__option_item!
        )

        set __result=!__result! !__opt_text!
    )

    set __options=%%b
    goto :init-compiler-options-loop
)

if not [!__result!] == [] (
    set __result=!__result:~1!
)

endlocal & set __last_init_compiler_options_result=%__result%
exit /b


:link
setlocal
set __include_path=%~1
if [!__include_path:~-1!] == [\] (
    set __include_path=!__include_path:~,-1!
)
call :clean "!__include_path!\"

call :tail %*
set __headers=!__last_tail_result!

:link-loop
for /f "usebackq eol= tokens=1* delims= " %%a in ('!__headers!') do (
    set __header_item=%%~a
    set __header_path=%%~dpnxa

    set __hdr_attr=%%~aa
    set __dir_attr=!__hdr_attr:~,1!

    if [!__dir_attr!] == [d] (
        for /f "usebackq eol= tokens=* delims=" %%a in (`where /r "!__header_path!" *.h 2^>nul`) do (
            call :mklink "!__include_path!" "%%~dpnxa" "!__header_item!"
        )
    ) else (if [!__dir_attr!] == [-] (
        call :mklink "!__include_path!" "!__header_path!" ""
    ))

    set __headers=%%b
    goto :link-loop
)
endlocal
exit /b


:mklink
setlocal
set __relative_path=%~2
set __relative_path=!__relative_path:%~dp0%~3\=!

call :count !__relative_path! \
set __count=%__last_count_result%

call :count "%~1" \
set /a __count+=%__last_count_result%+1

call :mult ..\ %__count%
set __mult_path=%__last_mult_result%

set __link_path=%~dpnx1\!__relative_path!
set __targ_path=%__mult_path%%~3\!__relative_path!

md !__link_path:\%~nx2=! 1>nul 2>nul
mklink !__link_path! !__targ_path! 1>nul 2>nul
endlocal
exit /b


:clean
setlocal
set __root_path=!cd!

call :trim %~1
set __remv_path=%~dp0!__last_trim_result!
if not [!__remv_path:~-1!] == [\] (
    set __remv_path=!__remv_path:%~nx1=!
)
set __remv_path="!__remv_path!"

cd !__remv_path! 1>nul 2>nul
if [!cd!] == [!__root_path!] (
    endlocal
    exit /b
)

cd !__root_path! 1>nul 2>nul

del /s /q /f !__remv_path! 1>nul 2>nul
rd /s /q !__remv_path! 1>nul 2>nul
endlocal
exit /b


:init-build-properties
setlocal
set __config_prefix=%~2
if [!__config_prefix!] == [] (
    set __config_prefix=config
)

for /f "usebackq eol=# tokens=1* delims=" %%a in ("%~dpnx1") do ^
for /f "usebackq eol= tokens=1* delims==" %%a in ('%%a') do (
    set __prop_name=__%__config_prefix%.%%a

    call :expand-variable !__prop_name!
    set __prop_value=!__last_expand_variable_result!

    call :trim !__prop_value! %%b
    set __prop_value=!__last_trim_result!

    if [!__prop_value:~-1!] == [\] (
        set __prop_value=!__prop_value:~,-1!
    )

    set !__prop_name!=!__prop_value!
    set __config_members=!__prop_name!%\n%!__config_members!
)

set __local_members=
for /f "usebackq eol= tokens=1* delims=" %%a in ('!__config_members!') do (
    echo;!__local_members! | (
        findstr /i "\<%%a\>" 1>nul 2>nul
    ) || set __local_members=%%a%\n%!__local_members!
)

set __config_members=!__local_members!
for /f "usebackq eol= tokens=1* delims=" %%a in ('!__config_members!') do (
    set __prop_name=%%a

    set __result_template=!__result_template!%\n%^
set !__prop_name:~1!=!%%a!
)

set __result_template=!__result_template!%\n%

if [!_flag.is_debug!] == [1] (
    echo;%\n%%\n%^
INIT-PROPS#TEMPLATE%\n%^
===================%\n%^
!__result_template:~1,-1!
)
endlocal & (%__result_template%)
exit /b


:init-build-flags
setlocal
call :init-array __long_flags "%~1"
call :init-array __short_flags "%~2"

for /l %%a in (%__long_flags.begin%, 1, %__long_flags.end%) do (
    set __long_flag=!__long_flags[%%a]!
    set __short_flag=!__short_flags[%%a]!

    set __flag_name=!__long_flag:~1!

    set __flags_template=!__flags_template!%\n%^
if /i [#] == [!__long_flag!] set __flag.is_!__flag_name!=1

    if not [!__short_flag!] == [] (
        set __flags_template=!__flags_template!%\n%^
if /i [#] == [!__short_flag!] set __flag.is_!__flag_name!=1
    )
)

set __flags_template=!__flags_template!%\n%

call :init-array __args "%~3"
for /l %%a in (%__args.begin%, 1, %__args.end%) do (
    call :execute-template __flags_template # !__args[%%a]!
)

for /l %%a in (%__long_flags.begin%, 1, %__long_flags.end%) do (
    set __flag=!__long_flags[%%a]!
    set __flag_name=!__flag:~1!

    call :expand-variable __flag.is_!__flag_name!
    set __flag_value=!__last_expand_variable_result!

    set __result_template=!__result_template!%\n%^
set _flag.is_!__flag_name!=!__flag_value!
)

set __result_template=!__result_template!%\n%

if [!__flag.is_debug!] == [1] (
    call :print-array __args
    echo;%\n%^
INIT-ARRAY#RESULT%\n%^
=================%\n%^
!__last_print_array_result!

    call :print-array __long_flags
    echo;%\n%%\n%^
INIT-ARRAY#RESULT%\n%^
=================%\n%^
!__last_print_array_result!

    call :print-array __short_flags
    echo;%\n%%\n%^
INIT-ARRAY#RESULT%\n%^
=================%\n%^
!__last_print_array_result!

    echo;%\n%%\n%^
TEST-FLAGS#TEMPLATE%\n%^
===================%\n%^
!__flags_template:~1,-1!

    echo;%\n%%\n%^
INIT-FLAGS#TEMPLATE%\n%^
===================%\n%^
!__result_template:~1,-1!
)
endlocal & (%__result_template%)
exit /b


:expand-variable
set __last_expand_variable_result=!%~1!
exit /b


:execute-template
setlocal
call :replace-template %1 %2 %3
endlocal & (%__last_replace_template_result%)
exit /b


:replace-template
set __last_replace_template_result=!%~1:%~2=%~3!
exit /b


:init-array
setlocal
set __arr_name=%~1
set __arg_count=0

set __next_arguments=%~2

:init-array-loop
for /f "usebackq eol= tokens=1* delims= " %%a in ('!__next_arguments!') do (
    set __arg_value=%%~a
    if [!__arg_value!] == [^^] (
        set __arg_value=
    )

    set __result_template=!__result_template!%\n%^
set __last_init_argument_array_result[%__arg_count%]=!__arg_value!

    set __next_arguments=%%b

    set /a __arg_count+=1
    goto :init-array-loop
)

set __result_template=!__result_template!%\n%^
set __last_init_argument_array_result.count=%__arg_count%%\n%^
set __last_init_argument_array_result.begin=0%\n%^
set /a __last_init_argument_array_result.end=%__arg_count%-1%\n%

set __result_template=!__result_template:^^%\n%=^!
set __result_template=!__result_template:^(=!
set __result_template=!__result_template:^)=!

endlocal & (%__result_template%) & call :copy-array %__arr_name% __last_init_argument_array_result
exit /b


:copy-array
set %~1.count=!%~2.count!
set %~1.begin=!%~2.begin!
set %~1.end=!%~2.end!

for /l %%a in (!%~1.begin!, 1, !%~1.end!) do (
    set %~1[%%a]=!%~2[%%a]!
)
exit /b


:print-array
set __last_print_array_result=
for /l %%a in (!%~1.begin!, 1, !%~1.end!) do (
    set __last_print_array_result=!__last_print_array_result!%\n%^
%~1[%%a]="!%~1[%%a]!"
)
set __last_print_array_result=!__last_print_array_result:~1!
exit /b


:mult
setlocal
set __string=%~1
set __count=%~2

set __result=
for /l %%a in (1, 1, %__count%) do (
    set __result=!__result!%__string%
)

endlocal & set __last_mult_result=%__result%
exit /b


:count
setlocal
set __test_path=%~1
set __split_path=!__test_path:%~2=#!

call :trim !__split_path!
set __split_path=!__last_trim_result!

set __count=-1

:count-loop
set __temp_path=!__split_path!
set __split_path=!__split_path:*#=!

set /a __count+=1
if not [!__split_path!] == [!__temp_path!] (
    goto :count-loop
)
endlocal & set __last_count_result=%__count%
exit /b


:tail
for /f "usebackq eol= tokens=1* delims= " %%a in ('%*') do (
    set __last_tail_result=%%b
)
exit /b


:trim
set __last_trim_result=%*
exit /b
