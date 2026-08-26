import React, {useState, useEffect, JSX} from 'react';

interface DownloadButtonProps{
    owner: String;
    repo: String;
}

type OS = 'Windows' | 'Linux' | 'Android' | 'Unknown';

export default function DownloadButton({owner,repo}:DownloadButtonProps): JSX.Element {
    const [os, setOs] = useState<OS>('Unknown');
    const [downloadUrl, setDownloadUrl] = useState(`https://github.com/${owner}/${repo}/releases/download`);

    return (
        //https://github.com/DCharVault/DCharVault/releases/download/v1.2.0-alpha/DCharVault-Windows.zip
        <a href={`https://github.com/DCharVault/DCharVault/releases/download/v1.2.0-alpha/${fileName}`}
           className="button button--secondary button--lg"
           >
            Download Portable Version
        </a>
    );
}