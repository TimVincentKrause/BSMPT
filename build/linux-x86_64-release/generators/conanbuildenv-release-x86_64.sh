script_folder="/home/kijetesantakalu/thesis/testBSMPT2/BSMPT/build/linux-x86_64-release/generators"
echo "echo Restoring environment" > "$script_folder/deactivate_conanbuildenv-release-x86_64.sh"
for v in PATH
do
    is_defined="true"
    value=$(printenv $v) || is_defined="" || true
    if [ -n "$value" ] || [ -n "$is_defined" ]
    then
        echo export "$v='$value'" >> "$script_folder/deactivate_conanbuildenv-release-x86_64.sh"
    else
        echo unset $v >> "$script_folder/deactivate_conanbuildenv-release-x86_64.sh"
    fi
done


export PATH="/home/kijetesantakalu/.conan2/p/cmakee300763ec076c/p/bin:$PATH"