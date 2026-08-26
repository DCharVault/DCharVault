import React, {useState, useEffect, JSX} from 'react';
import {data} from "browserslist";

interface DownloadButtonProps{
    owner: String;
    repo: String;
}

type OS = 'Windows' | 'Linux' | 'Android' | 'Unknown';

export default function DownloadButton({owner,repo}:DownloadButtonProps): JSX.Element {
    const [os, setOs] = useState<OS>('Unknown');
    const [downloadUrl, setDownloadUrl] = useState(`https://github.com/${owner}/${repo}/releases/download`);
    const [version, setVersion] = useState<string>('');
    const [loading, setLoading] = useState<boolean>(true);
    useEffect(()=>{
       const userAgent = window.navigator.userAgent;
       let detectOS: OS='unknown';
       if(userAgent.indexOf('Win')!==-1) detectOS = 'Windows';
       else if(userAgent.indexOf('Linux')!==-1) detectOS = 'Linux';
       else if(userAgent.indexOf('Android')!==-1) detectOS = 'Android';

       setOs(detectOS);

        fetch(`http://api.github.com/repos/${owner}/${repo}/releases/download`)
            .then((res) => res.json())
            .then((data)=> {
                if (data.tag_name) {
                    setVersion(data.tag_name);
                }

                const assets = data.assets || [];
                let targetAssets;
                if (detectOS === 'Windows') {
                    targetAssets = assets.find((a: any) => a.name.endsWith('.exe'));
                } else if (detectOS === 'linux') {
                    targetAssets = assets.find((a: any) => a.name.endsWith('.zip'));
                }

                if (targetAssets) {
                    setDownloadUrl(targetAssets.browser_download_url);
                }
                setLoading(false);
            })
            .catch((err)=>{
               console.error('Failed to Fetch download releases',err);
            });
    }, [owner,repo]);

    const buttonText = loading ? 'Checking latest version...' : version
    return (
        //https://github.com/DCharVault/DCharVault/releases/download/v1.2.0-alpha/DCharVault-Windows.zip
        <a href={`https://github.com/DCharVault/DCharVault/releases/download/v1.2.0-alpha/${fileName}`}
           className="button button--secondary button--lg"
           >
            Download Portable Version
        </a>
    );
}