import React, { type ReactNode } from 'react';
import clsx from 'clsx';
import Layout from '@theme/Layout';
import Link from '@docusaurus/Link';
import useDocusaurusContext from '@docusaurus/useDocusaurusContext';
import Heading from '@theme/Heading';

type FeatureItem = {
  title: string;
  description: ReactNode;
  link: string;
};


const FeatureList: FeatureItem[] = [
  {
    title: 'Secure Architecture',
    description: (
      <>
        Dive into the <code>EncryptionManager</code>, secure memory allocators, 
        and session timeout implementations that keep the vault secure.
      </>
    ),
    link: '/docs/ARCHITECTURE',
  },
  {
    title: 'Core Models',
    description: (
      <>
        Understand the foundational C++ structures backing the app, including 
        Diary Entries, Databases, and the main Vault Model.
      </>
    ),
    link: '/docs/CORE_MODELS',
  },
  {
    title: 'UI & ViewModels',
    description: (
      <>
        Explore the QML desktop/Android interfaces and the reactive ViewModels 
        like the <code>RichTextController</code> and clipboard sanitizers.
      </>
    ),
    link: '/docs/VIEW_MODELS',
  },
];

function Feature({title, description, link}: FeatureItem) {
  return (
    <div className={clsx('col col--4')}>
      <div className="text--center padding-horiz--md padding-vert--md">
        <Heading as="h3">{title}</Heading>
        <p>{description}</p>
        <Link className="button button--secondary button--sm" to={link}>
          Read More &rarr;
        </Link>
      </div>
    </div>
  );
}

function HomepageHeader() {
  const {siteConfig} = useDocusaurusContext();
  return (
    <header className={clsx('hero hero--primary')} style={{ padding: '4rem 0', textAlign: 'center' }}>
      <div className="container">
        <Heading as="h1" className="hero__title">
          {siteConfig.title}
        </Heading>
        <p className="hero__subtitle">{siteConfig.tagline}</p>
        <div style={{ marginTop: '2rem' }}>
          <Link
            className="button button--secondary button--lg"
            to="/docs/ARCHITECTURE">
            Explore Documentation
          </Link>
        </div>
      </div>
    </header>
  );
}

export default function Home(): ReactNode {
  const {siteConfig} = useDocusaurusContext();
  return (
    <Layout
      title={`${siteConfig.title} Documentation`}
      description="Technical documentation for DCharVault - Secure, encrypted diary and vault application">
      <HomepageHeader />
      <main>
        <section className="padding-vert--xl">
          <div className="container">
            <div className="row">
              {FeatureList.map((props, idx) => (
                <Feature key={idx} {...props} />
              ))}
            </div>
          </div>
        </section>
      </main>
    </Layout>
  );
}