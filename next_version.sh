#!/bin/bash


describe=$(eval git describe --always --tags)


if [[ $describe =~ ^[a-z0-9]+$ ]]; then
	next_version="0.1.0"
elif [[ $describe =~ ^[0-9].[0-9].[0-9]$ ]]; then
	next_version=$describe
elif [[ $describe =~ ^([0-9].[0-9].[0-9])-([0-9])-[a-z0-9]+$ ]]; then
	commits_since_tag=${BASH_REMATCH[2]}
	version_tag=${BASH_REMATCH[1]}
	
	IFS=. read MAJOR MINOR PATCH <<<"${version_tag}"
	
	NEXT_MAJOR=$(eval echo $(($MAJOR+1)))
	NEXT_MINOR=$(eval echo $(($MINOR+1)))
	NEXT_PATCH=$(eval echo $(($PATCH+1)))
	
	last_commits=$(eval git log -n $commits_since_tag --oneline)
	
	if [[ $last_commits == *"[hdw]"* ]] || [[ $last_commits == *"[ml]"* ]]; then
		next_version="$NEXT_MAJOR.0.0"
	elif [[ $last_commits == *"[feat]"* ]] || [[ $last_commits == *"[training]"* ]]; then
		next_version="$MAJOR.$NEXT_MINOR.0"
	else
		next_version="$MAJOR.$MINOR.$NEXT_PATCH"
	fi
else
	echo "ERROR: Unexpected format from git describe" 1>&2
	exit 1
fi

echo $next_version
