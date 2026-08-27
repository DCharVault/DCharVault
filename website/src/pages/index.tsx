import React, { type ReactNode } from 'react';
import clsx from 'clsx';
import Layout from '@theme/Layout';
import Link from '@docusaurus/Link';
import useDocusaurusContext from '@docusaurus/useDocusaurusContext';
import Heading from '@theme/Heading';
import DownloadButton from "@site/src/components/downloadButton";

type FeatureItem = {
  title: string;
  description: ReactNode;
  link: string;
  badge: string;
};

const FeatureList: FeatureItem[] = [
  {
    title: 'Architecture',
    badge: 'MVVM · C++20 · Qt 6.8',
    description: (
      <>
        Understand the MVVM layering, data flow from QML to SQLite, and how the
        cryptographic pipeline prevents plaintext from ever touching disk.
      </>
    ),
    link: '/docs/ARCHITECTURE',
  },
  {
    title: 'Core Models API',
    badge: 'DiaryManager · EncryptionManager · SecureAllocator',
    description: (
      <>
        Full API reference for the C++ backend — vault lifecycle, CRUD operations,
        Argon2id key derivation, XChaCha20-Poly1305 encryption, and secure memory types.
      </>
    ),
    link: '/docs/CORE_MODELS',
  },
  {
    title: 'ViewModel API',
    badge: 'QObject · Signals · Q_INVOKABLES',
    description: (
      <>
        The QML-facing bridge layer — <code>LoginViewModel</code>, <code>DiaryListModel</code>,
        clipboard sanitization, rich text editing, and secure password input hooks.
      </>
    ),
    link: '/docs/VIEW_MODELS',
  },
];

function Feature({title, description, link, badge}: FeatureItem) {
  return (
    <div className={clsx('col col--4')}>
      <div className="text--center padding-horiz--md padding-vert--md">
        <div style={{ marginBottom: '0.5rem' }}>
          <span style={{
            fontSize: '0.7rem',
            fontWeight: 700,
            letterSpacing: '0.08em',
            textTransform: 'uppercase',
            color: 'var(--ifm-color-primary)',
            opacity: 0.85,
          }}>
            {badge}
          </span>
        </div>
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
        <div style={{ marginTop: '2rem', display: 'flex', gap: '1rem', justifyContent: 'center', flexWrap: 'wrap' }}>
          <Link
            className="button button--secondary button--lg"
            to="/docs/intro">
            Get Started →
          </Link>
          <div>
            <DownloadButton owner='DCharVault' repo='DCharVault'/>
          </div>
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
      description="Technical documentation for DCharVault - Secure, zero-knowledge, encrypted diary and journaling application built on C++20 and Qt 6.8">
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