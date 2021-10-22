import { useMemo } from 'react'
import { Card, DataGrid } from 'rootnet-ui'
import { DownCsv } from '../../../../../common/components'

export function curryList({ title, useColumns, useGetData }) {
  return function List({ params }) {
    const { data, error, loading } = useGetData(params)
    const columns = useColumns()
    const extra = <DownCsv tit={title} value={data} options={columns} label text='下载' />

    return (
      <Card title={title} className='flex-y x-card-singlegrid' error={error} loading={loading} extra={extra}>
        <DataGrid option={useOption(columns)} data={data} />
      </Card>
    )
  }
}

function useOption(columns) {
  return useMemo(() => {
    return {
      resizable: true,
      virtualized: false,
      autoFill: true,
      fixedLeft: 1,
      columns,
      nilText: '-',
      emptyText: '-',
    }
  }, [columns])
}
