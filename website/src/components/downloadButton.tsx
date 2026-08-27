import React, { useState, useEffect, JSX } from 'react';
// Removed unused 'browserslist' import

interface DownloadButtonProps {
    owner: string; // Changed from String to string
    repo: string;  // Changed from String to string
}

type OS = 'Windows' | 'Linux' | 'Android' | 'Unknown';

export default function DownloadButton({ owner, repo }: DownloadButtonProps): JSX.Element {
    const [os, setOs] = useState<OS>('Unknown');
    // Default to the main releases page in case the fetch fails
    const [downloadUrl, setDownloadUrl] = useState(`https://github.com/${owner}/${repo}/releases`);
    const [version, setVersion] = useState<string>('');
    const [loading, setLoading] = useState<boolean>(true);

    useEffect(() => {
        const userAgent = window.navigator.userAgent;
        let detectOS: OS = 'Unknown'; // Capitalized to match type

        if (userAgent.indexOf('Win') !== -1) detectOS = 'Windows';
        else if (userAgent.indexOf('Linux') !== -1) detectOS = 'Linux';
        else if (userAgent.indexOf('Android') !== -1) detectOS = 'Android';

        setOs(detectOS);

        // Fixed API endpoint to hit /releases/latest and used https
        fetch(`https://api.github.com/repos/${owner}/${repo}/releases/latest`)
            .then((res) => res.json())
            .then((data) => {
                if (data.tag_name) {
                    setVersion(data.tag_name);
                }

                const assets = data.assets || [];
                let targetAsset;
                
                if (detectOS === 'Windows') {
                    targetAsset = assets.find((a: any) => a.name.endsWith('.exe'));
                } else if (detectOS === 'Linux') { // Capitalized to match type
                    targetAsset = assets.find((a: any) => a.name.endsWith('.zip'));
                }

                if (targetAsset) {
                    setDownloadUrl(targetAsset.browser_download_url);
                }
                setLoading(false);
            })
            .catch((err) => {
                console.error('Failed to fetch download releases', err);
                setLoading(false); // Stop loading even on error
            });
    }, [owner, repo]);

    // Fixed template literal (swapped single quotes for backticks)
    const buttonText = loading 
        ? 'Checking latest version...' 
        : version 
            ? `Download ${version} for ${os !== 'Unknown' ? os : 'Desktop'}`
            : 'Download latest version';

    return (
        // Wrapped adjacent elements in a React Fragment
        <>
            <a 
                href={downloadUrl}
                className="button button--secondary button--lg"
            >
                {buttonText}
            </a>

            {/* Fixed JSX template literal syntax with curly braces */}
            <a href={`https://github.com/${owner}/${repo}/releases/`}>
                View All Releases
            </a>
        </>
    );
}